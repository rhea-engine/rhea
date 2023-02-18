#include <Psio/Net/IOBuffer.h>

#include "ServerStream.h"
#include <Opcode.h>
#include <Psio/Log.h>

using Psio::Network;
using Psio::IOBuffer;

namespace Hermes
{
	int ServerStream::onLogin(MessageHandler handler)
	{
		m_message_handler = handler;

		m_inbuf = std::make_unique<Psio::IOBuffer>(64 * 1024);
		m_outbuf = std::make_unique<Psio::IOBuffer>(64 * 1024);

		m_next_state = HERMES_LOGIN_PENDING;

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

	int ServerStream::DoLoop(int result)
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
		if (result < 0)
			return result;
		
		// check if message is received completely
		std::unique_ptr<Message> msg;
		msg.reset(m_msg_factory->getMessage(m_inbuf.get()));
		if (msg) {
			if (msg->getOpcode() != HERMES_MSG_LOGIN) {
				// wrong message					
				// note this will probably destroy the stream
				m_message_handler(nullptr, HERMES_ERR_GENERAL);
				return Network::IO_PENDING;
			}
			else {
				m_message_handler(std::move(msg), 0);
				return Network::IO_PENDING;
			}
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