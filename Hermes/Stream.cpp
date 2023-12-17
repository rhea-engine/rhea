#include "Stream.h"
#include <Psio/Log.h>

namespace Hermes
{
	int Stream::sendMessage(Message* message, CompletionHandler handler)
	{
		message->serialize(m_outbuf.get());
		static uint16_t lastopcode = 0;
		static uint16_t repeat = 0;
		if (lastopcode != message->base.opcode) {
			DLOG(LOG_DEBUG) << "[" << m_id << "] Sending msg " << message->base.opcode << ". Size=" << m_outbuf->Remaining();
			if (repeat)
				DLOG(LOG_DEBUG) << "[" << m_id << "] repeated " << repeat << " times";
			lastopcode = message->base.opcode;
			repeat = 0;
		}
		else
			repeat++;
		return m_sock->Write(m_outbuf.get(), [handler](int err) {
			if (handler) {
				handler(err);
			}
			});
	}

	int Stream::Start()
	{
		m_next_state = HERMES_READMESSAGE;

		Psio::SocketAddress* addr = m_sock->getRemoteAddress();
		if (addr)
			m_id = addr->GetHost() + ":" + std::to_string(addr->GetPort());
		else
			m_id = "remote";

		int ret = DoLoop(1);

		return ret;
	}

	int Stream::Restart()
	{
		return DoLoop(1);
	}

	void Stream::Close()
	{
		m_next_state = HERMES_CLOSE;
	}

	int Stream::DoLoop(int result)
	{
		do
		{
			m_state = m_next_state;
			m_next_state = HERMES_NONE;

			switch (m_state)
			{
			case HERMES_READMESSAGE:
				result = DoReadMessage(result);
				break;
			case HERMES_READMESSAGE_COMPLETED:
				result = DoReadMessageCompleted(result);
				break;
			case HERMES_CLOSE:
				result = DoClose(result);
				break;
			}

		} while (result != Psio::Network::IO_PENDING && m_state != HERMES_NONE);
		return result;
	}

	void Stream::CallDoLoop(int result)
	{
		int res = DoLoop(result);
		//if ((res != Psio::Network::Err::IO_PENDING) && (m_message_handler != NULL))
		//	m_message_handler(nullptr, result);
	}

	int Stream::DoReadMessage(int result)
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

	int Stream::DoReadMessageCompleted(int result)
	{
		if (result <= 0) {
			// call handler with error code - this could destroy |this|			
			if (result != -1)
				m_err_handler(result);
			return -1;
		}

		// check if message is received completely
		Message* msg;
		msg = m_msg_factory->getMessage(m_inbuf.get());
		if (msg) {
			LOG(LOG_DEBUG) << "[" << m_id << "] Message received: " << msg->base.opcode << " size=" << msg->base.size;
			int res = 0;
			if (m_inbuf->Remaining()) {
				m_next_state = HERMES_STATE::HERMES_READMESSAGE_COMPLETED;
				res = (int)m_inbuf->Remaining();
			}
			else {
				m_next_state = HERMES_STATE::HERMES_READMESSAGE;
				res = 0;
			}
			// the message handler must not destroy |this|
			// it may set next state to 'close' though
			m_message_handler(msg);
			return res;
		}
		else {
			m_next_state = HERMES_STATE::HERMES_READMESSAGE;
			return 0;
		}
	}

	int Stream::DoClose(int result)
	{
		m_next_state = HERMES_NONE;
		// this may destroy |this|
		if (m_closed_handler)
			m_closed_handler(0);
		return Psio::Network::IO_PENDING;
	}
}