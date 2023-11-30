#include <Psio/Net/IOBuffer.h>

#include "ServerStream.h"
#include <Opcode.h>
#include <Psio/Log.h>
#include <Hermes/Messages/MessageLogin.h>
#include <Messages/MessageConnect.h>
#include <Hermes/Messages/MessageLoginFailed.h>

using Psio::Network;
using Psio::IOBuffer;

namespace Hermes
{
	int ServerStream::onLogin(LoginHandler handler, CompletionHandler err_handler)
	{
		m_login_handler = handler;
		m_err_handler = err_handler;

		m_inbuf = std::make_unique<Psio::IOBuffer>(64 * 1024);
		m_outbuf = std::make_unique<Psio::IOBuffer>(64 * 1024);

		m_next_state = HERMES_CONNECT_PENDING;

		int ret = DoLoop(0);

		return ret;
	}

	int ServerStream::onMessage(MessageHandler handler)
	{
		m_message_handler = handler;

		m_next_state = HERMES_READMESSAGE_COMPLETED;

		int ret = DoLoop(0);

		return ret;
	}

	int ServerStream::LoginRetry()
	{
		m_next_state = HERMES_LOGIN_PENDING;
		return DoLoop(0);
	}

	void ServerStream::AuthSucess(CompletionHandler handler)
	{
		Hermes::Message msg_auth(HERMES_MSG_LOGIN_SUCCEEDED);
		sendMessage(&msg_auth, [&](int result) {
			handler(result);
			});
	}

	void ServerStream::AuthFailure(CompletionHandler handler, std::vector<std::string> methods)
	{
		Hermes::MessageLoginFailed msg_auth;
		msg_auth.methods = methods;

		sendMessage(&msg_auth, [&](int result) {
			handler(result);
			});
	}

	int ServerStream::DoLoop(int result)
	{
		do
		{
			m_state = m_next_state;
			m_next_state = HERMES_NONE;

			switch (m_state)
			{
			case HERMES_CONNECT_PENDING:
				result = DoConnectPending(result);
				break;
			case HERMES_CONNECT_COMPLETED:
				result = DoConnectCompleted(result);
				break;
			case HERMES_LOGIN_PENDING:
				result = DoLoginPending(result);
				break;
			case HERMES_LOGIN_COMPLETED:
				result = DoReadLoginCompleted(result);
				break;
			case HERMES_READMESSAGE:
				result = DoReadMessage(result);
				break;
			case HERMES_READMESSAGE_COMPLETED:
				result = DoReadMessageCompleted(result);
				break;
			}

		} while (result != Network::IO_PENDING && m_state != HERMES_NONE);

		return result;
	}

	void ServerStream::CallDoLoop(int result)
	{
		int res = DoLoop(result);
	//	if ((res != Network::Err::IO_PENDING) && (m_message_handler != NULL))
	//		m_message_handler(res, result);
	}

	int ServerStream::DoConnectPending(int result)
	{
		if (result < 0)
			return result;

		m_next_state = HERMES_STATE::HERMES_CONNECT_COMPLETED;

		return m_sock->Read(m_inbuf.get(), [&](int result) -> int {
			{
				CallDoLoop(result);
				return -1;
			}
			});
	}

	int ServerStream::DoConnectCompleted(int result)
	{
		if (result < 0)
			return result;

		// check if message is received completely
		std::unique_ptr<Message> msg;
		msg.reset(m_msg_factory->getMessage(m_inbuf.get()));
		if (msg) {
			// check if connect message received
			if (msg->getOpcode() != HERMES_MSG_CONNECT) {
				// wrong message									
				Message res_msg(HERMES_ERR_GENERAL);
				m_next_state = HERMES_STATE::HERMES_CONNECT_PENDING;
				sendMessage(&res_msg, [&](int err) {
					// note this will probably destroy |this|
					m_err_handler(-HERMES_ERR_GENERAL);
					});
				return Network::IO_PENDING;
			}
			MessageConnect* msg_connect = static_cast<MessageConnect*>(msg.get());
			// check if protocol version is available
			if (msg_connect->protocol_version != HERMES_PROTOCOL_VERSION) {
				// wrong protocol version
				m_next_state = HERMES_STATE::HERMES_CONNECT_PENDING;
				Message res_msg(HERMES_ERR_PROTOCOL_VERSION);
				sendMessage(&res_msg, [&](int err) {
					// note this will probably destroy |this|
					m_err_handler(-HERMES_ERR_PROTOCOL_VERSION);
					});
				return Network::IO_PENDING;
			}

			// connection established
			Message res_msg(HERMES_MSG_CONNECTED);
			sendMessage(&res_msg, [&](int result) -> int {
				{
					m_next_state = HERMES_STATE::HERMES_LOGIN_PENDING;
					CallDoLoop(result);
					return -1;
				}
				});
			return Network::IO_PENDING;

		/*	// wait for login message
			if (m_inbuf->Remaining() > 0) {
				m_next_state = HERMES_STATE::HERMES_LOGIN_COMPLETED;
			}
			else {
				m_next_state = HERMES_STATE::HERMES_LOGIN_PENDING;				
			}
			return (int)m_inbuf->Remaining();*/
		}
		else {
			// no message received
			m_next_state = HERMES_STATE::HERMES_CONNECT_PENDING;
			return result;
		}
	}

	int ServerStream::DoLoginPending(int result)
	{
		if (result < 0)
			return result;

		m_next_state = HERMES_STATE::HERMES_LOGIN_COMPLETED;

		return m_sock->Read(m_inbuf.get(), [&](int result) -> int {
			{
				CallDoLoop(result);
				return -1;
			}
			});
	}

	int ServerStream::DoReadLoginCompleted(int result)
	{
		if (result < 0) {
			if (result < -1)
				m_err_handler(result);
			return result;
		}
		
		// check if message is received completely
		std::unique_ptr<Message> msg;
		msg.reset(m_msg_factory->getMessage(m_inbuf.get()));
		if (msg) {
			if (msg->getOpcode() != HERMES_MSG_LOGIN) {
				// wrong message									
				Message msg(HERMES_ERR_GENERAL);
				sendMessage(&msg, [&](int err) {
					// note this will probably destroy |this|
					m_err_handler(-HERMES_ERR_GENERAL);
					});
				return Network::IO_PENDING;
			}
			MessageLogin* msg_login = static_cast<MessageLogin*>(msg.get());
		
			// login message received
			// on failure may destroy |this|
			m_login_handler(msg_login->auth_method, msg_login->user_name, msg_login->secrets);
			return Network::IO_PENDING;

		}
		else {
			m_next_state = HERMES_STATE::HERMES_LOGIN_PENDING;
			return 0;
		}
	}

	int ServerStream::DoReadMessage(int result)
	{
		if (result < 0)
			return result;

		m_next_state = HERMES_STATE::HERMES_READMESSAGE_COMPLETED;

		return m_sock->Read(m_inbuf.get(), [&](int result) -> int {
			{
				m_next_state = HERMES_STATE::HERMES_READMESSAGE_COMPLETED;
				CallDoLoop(result);
				return -1;
			}
			});
	}

	int ServerStream::DoReadMessageCompleted(int result)
	{
		if (result < 0) {
			// call handler with error code - this could destroy |this|
			m_message_handler(nullptr, result);
			return -1;
		}

		// check if message is received completely
		std::unique_ptr<Message> msg;
		msg.reset(m_msg_factory->getMessage(m_inbuf.get()));
		if (msg) {
			DLOG(LOG_DEBUG) << "Received message: " << msg->base.opcode;
			m_message_handler(std::move(msg), 0);
			if (m_inbuf->Remaining()) {
				m_next_state = HERMES_STATE::HERMES_READMESSAGE_COMPLETED;
				return (int)m_inbuf->Remaining();
			}
			else {
				m_next_state = HERMES_STATE::HERMES_READMESSAGE;
				return 0;
			}			
		}	
		else {
			m_next_state = HERMES_STATE::HERMES_READMESSAGE;
			return 0;
		}
	}

}