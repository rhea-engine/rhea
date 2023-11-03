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

	void ClientStream::login(std::string user_name, LoginHandler handler)
	{
		m_login_handler = handler;
		Hermes::MessageLogin msg;
		msg.user_name = user_name;

		int ret = sendMessage((Message*)&msg, handler);
		if (ret >= msg.getSize()) {
			handler(ret);
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
				result = -1;// DoLoginPending(result);
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