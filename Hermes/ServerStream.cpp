#include <Psio/Net/IOBuffer.h>

#include "ServerStream.h"
#include <Opcode.h>
#include <Psio/Log.h>
#include <Hermes/Messages/MessageLogin.h>
#include <Messages/MessageConnect.h>
#include <Hermes/Messages/MessageLoginFailed.h>
#include <Messages/MessageCreateUser.h>

using Psio::Network;

namespace Hermes
{
	int ServerStream::Connect()
	{
		m_stream->onMessage([&](Message* msg) {
			m_msg.reset(msg);
			CallDoLoop(1);
			});
		m_stream->onClosed([&](int err) {
			if (m_closed_handler)
				m_closed_handler(err);
			});
		m_stream->onConnectionError([&](int err) {
			if (m_err_handler)
				m_err_handler(err);
			else
				m_stream->Close();
			});

		m_next_state = HERMES_CONNECT_PENDING;
		return DoLoop(1);
	}

	int ServerStream::LoginRetry()
	{
		m_next_state = HERMES_LOGIN_PENDING;
		return DoLoop(1);
	}

	void ServerStream::onLogin(LoginHandler handler)
	{
		m_login_handler = handler;		
	}

	void ServerStream::onMessage(MessageHandler handler)
	{
		m_message_handler = handler;
	}

	int ServerStream::sendMessage(Message* message, Stream::CompletionHandler handler)
	{
		// forward to stream
		return m_stream->sendMessage(message, handler);
	}

	void ServerStream::Close()
	{
		m_stream->Close();
	}

	int ServerStream::AuthSuccess(Stream::CompletionHandler handler)
	{
		Hermes::Message msg_auth(HERMES_MSG_LOGIN_SUCCEEDED);
		m_next_state = HERMES_READMESSAGE;

		int ret = m_stream->sendMessage(&msg_auth, [&](int result) {
			handler(result);
			});

		if (ret > -2)
			ret = 1;

		// listen to normal messages
		return DoLoop(ret);
	}

	int ServerStream::AuthFailure(Stream::CompletionHandler handler, std::vector<std::string> methods)
	{
		Hermes::MessageLoginFailed msg_auth;
		msg_auth.methods = methods;

		return m_stream->sendMessage(&msg_auth, [&](int result) {
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
	}

	int ServerStream::DoConnectPending(int result)
	{
		if (result < 0)
			return result;

		m_next_state = HERMES_STATE::HERMES_CONNECT_COMPLETED;

		return m_stream->Start();
	}

	int ServerStream::DoConnectCompleted(int result)
	{
		if (result < 0)
			return result;

		// check if message is received completely
		if (m_msg) {
			std::unique_ptr<Message> msg(std::move(m_msg));
			
			// check if connect message received
			if (msg->getOpcode() != HERMES_MSG_CONNECT) {
				// wrong message									
				Message res_msg(HERMES_ERR_GENERAL);
				m_next_state = HERMES_STATE::HERMES_CONNECT_PENDING;
				m_stream->sendMessage(&res_msg, [&](int err) {
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
				m_stream->sendMessage(&res_msg, [&](int err) {
					// note this will probably destroy |this|
					m_err_handler(-HERMES_ERR_PROTOCOL_VERSION);
					});
				return Network::IO_PENDING;
			}

			// connection established
			Message res_msg(HERMES_MSG_CONNECTED);
			m_stream->sendMessage(&res_msg, [&](int result) -> int {
				{
					m_next_state = HERMES_STATE::HERMES_LOGIN_PENDING;
					CallDoLoop(result);
					return -1;
				}
				});
			return Network::IO_PENDING;		
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

		return -1;
	}

	int ServerStream::DoReadLoginCompleted(int result)
	{
		if (result < 0) {
			if (result != -1)
				m_err_handler(result);
			return result;
		}
		
		// check if message is received completely
	
		if (m_msg) {
			std::unique_ptr<Message> msg(std::move(m_msg));

			// pending until auth success
			m_next_state = HERMES_STATE::HERMES_LOGIN_PENDING;
			
			// if create is allowed check for that message
			if (m_create_handler && msg->getOpcode() == HERMES_MSG_CREATE_USER)
			{
				MessageCreateUser* msg_create = static_cast<MessageCreateUser*>(msg.get());
				m_create_handler(msg_create->auth_method, msg_create->user_name, msg_create->name, msg_create->ext_id, msg_create->secrets);
				return Network::IO_PENDING;
			}
			// if not a login message this is a protocol error
			if (msg->getOpcode() != HERMES_MSG_LOGIN) {
				// wrong message									
				Message msg(HERMES_ERR_GENERAL);
				m_stream->sendMessage(&msg, [&](int err) {
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

		return -1;
	}

	int ServerStream::DoReadMessageCompleted(int result)
	{
		if (result < 0) {
			// call handler with error code - this could destroy |this|			
			if (result != -1)
				m_err_handler(result);
			return result;
		}

		// check if message is received completely
		if (m_msg) {
			std::unique_ptr<Message> msg(std::move(m_msg));

			m_next_state = HERMES_STATE::HERMES_READMESSAGE;

			// the message handler must not destroy |this|
			// it may set next state to 'close' though
			m_message_handler(std::move(msg));
			return 0;
		}
		else {
			m_next_state = HERMES_STATE::HERMES_READMESSAGE;
			return 0;
		}
	}
}