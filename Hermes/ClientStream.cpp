#include "ClientStream.h"
#include <Psio/Log.h>
#include <Opcode.h>
#include <Messages/MessageLogin.h>

namespace Hermes
{

	int ClientStream::connect(CompletionHandler handler)
	{
		m_inbuf = std::make_unique<Psio::IOBuffer>(64 * 1024);
		m_outbuf = std::make_unique<Psio::IOBuffer>(64 * 1024);

		return 0;
	}

	void ClientStream::login(std::string world_name, std::string user_name, LoginHandler handler)
	{
		m_login_handler = handler;

		Hermes::MessageLogin msg;
		msg.world_name = world_name;
		msg.user_name = user_name;
		m_next_state = HERMES_LOGIN_PENDING;

		int ret = sendMessage((Message*)&msg, [&](int err) {
			CallDoLoop(err);
			});
		if (ret >= msg.getSize()) {
			// synchronous sending - enter loop
			DoLoop(ret);
		}
	}

	int ClientStream::onMessage(MessageHandler handler)
	{
		m_message_handler = handler;

		m_next_state = HERMES_READMESSAGE;

		int ret = DoLoop(0);

		return ret;
	}
	
	int ClientStream::DoLoop(int result)
	{
		do
		{
			m_state = m_next_state;
			m_next_state = HERMES_NONE;

			switch (m_state)
			{
			case HERMES_LOGIN_PENDING:
				result = DoLoginPending(result);
				break;
			case HERMES_LOGIN_COMPLETED:
				result = DoLoginCompleted(result);
				break;
			case HERMES_READMESSAGE:
				result = DoReadMessage(result);
				break;
			case HERMES_READMESSAGE_COMPLETED:
				result = DoReadMessageCompleted(result);
			}

		} while (result != Psio::Network::IO_PENDING && m_state != HERMES_NONE);

		return result;
	}

	void ClientStream::CallDoLoop(int result)
	{
		int res = DoLoop(result);
		//	if ((res != Network::Err::IO_PENDING) && (m_message_handler != NULL))
		//		m_message_handler(res, result);
	}

	int ClientStream::DoLoginPending(int result)
	{
		if (result < 0)
			return result;

		m_next_state = HERMES_STATE::HERMES_LOGIN_COMPLETED;

		return m_sock->Read(m_inbuf.get(), [&](int result) -> int {		
				CallDoLoop(result);
				return -1;			
			});
	}

	int ClientStream::DoLoginCompleted(int result)
	{
		if (result < 0)
			return result;

		// check if message is received completely
		std::unique_ptr<Message> msg;
		msg.reset(m_msg_factory->getMessage(m_inbuf.get()));
		if (!msg) {
			m_next_state = HERMES_LOGIN_PENDING;
			return result;
		}

		// message received - what is it
		switch (msg->base.opcode) {
		case HERMES_ERR_GENERAL:
		case HERMES_PROTOCOL_VERSION:
		case HERMES_MSG_LOGIN_FAILED:
			m_login_handler(msg->base.opcode);
			break;
		case HERMES_MSG_LOGIN_SUCCEEDED:
			m_login_handler(0);
			break;
		}
		// exit DoLoop - wait for next instruction
		return -1;
	}

	int ClientStream::DoReadMessage(int result)
	{
		if (result < 0)
			return result;

		m_next_state = HERMES_STATE::HERMES_READMESSAGE_COMPLETED;

		return m_sock->Read(m_inbuf.get(), [&](int result) -> int {
			{
				CallDoLoop(result);
				return -1;
			}
			});
	}

	int ClientStream::DoReadMessageCompleted(int result)
	{
		if (result < 0) {
			// this could destroy |this|
			m_message_handler(nullptr, result);
			return -1;
		}

		// check if message is received completely
		std::unique_ptr<Message> msg;
		msg.reset(m_msg_factory->getMessage(m_inbuf.get()));
		if (msg) {
			DLOG(LOG_DEBUG) << "received message with opcode=" << msg->getOpcode();
			m_message_handler(std::move(msg), 0);
			if (m_inbuf->Remaining()) {
				m_next_state = HERMES_STATE::HERMES_READMESSAGE_COMPLETED;
				return m_inbuf->Remaining();
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