/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Hermes/Stream.h"
#include <Psio/Log.h>
#include <Hermes/Messages/MessageConnect.h>

namespace hermes
{
	int Stream::sendMessage(Message* message, CompletionHandler handler)
	{
		message->serialize(m_outbuf.get());
		static uint16_t lastopcode = 0;
		static uint16_t repeat = 0;
		if (lastopcode != message->base.opcode) {
			if (repeat)
				DLOG(LOG_DEBUG) << "[" << m_id << "] Msg " << lastopcode << " repeated " << repeat << " times";
			DLOG(LOG_DEBUG) << "[" << m_id << "] Sending msg " << message->base.opcode << " (" << m_msg_factory.getMessageName(message->getOpcode()) << "). Size=" << m_outbuf->Remaining();
			lastopcode = message->base.opcode;
			repeat = 0;
		}
		else
			repeat++;
		int ret = m_sock->Write(m_outbuf.get(), [&, handler](int err) {
			if (err < -1 && m_err_handler)
				m_err_handler(err);
			if (handler) {
				handler(err);
			}
			});
		if (ret < -1 && m_err_handler)
			m_err_handler(ret);
		return ret;
	}

	int Stream::Start()
	{
		m_next_state = HERMES_READMESSAGE;

		// connect handler
		m_msg_factory.registerMessageHandler<MessageConnect>(HERMES_MSG_CONNECT,
			[&](uint16_t opcode)->bool {
				return !m_connected;
			},
			std::bind(&Stream::onConnect, this, std::placeholders::_1)
		);

		psio::SocketAddress* addr = m_sock->getRemoteAddress();
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

	int Stream::Connect(std::string world_name, CompletionHandler handler)
	{
		m_on_connection_handler = handler;

		m_msg_factory.registerMessageHandler<Message>(HERMES_MSG_CONNECTED,
			[&](uint16_t opcode)->bool {
				return !m_connected;
			},
			std::bind(&Stream::onConnectedMessage, this, std::placeholders::_1)
		);
		// error handling
		m_msg_factory.registerMessageHandler<Message>(HERMES_ERR_GENERAL,
			nullptr,
			std::bind(&Stream::onErrorMessage, this, std::placeholders::_1)
		);
		m_msg_factory.registerMessageHandler<Message>(HERMES_ERR_PROTOCOL_VERSION,
			nullptr,
			std::bind(&Stream::onErrorMessage, this, std::placeholders::_1)
		);

		hermes::MessageConnect msg;
		msg.world_name = world_name;

		m_next_state = HERMES_READMESSAGE;

		psio::SocketAddress* addr = m_sock->getRemoteAddress();
		if (addr)
			m_id = addr->GetHost() + ":" + std::to_string(addr->GetPort());
		else
			m_id = "remote";

		int ret = sendMessage((Message*)&msg, [&](int err) {
			if (err < 0 && m_err_handler) {
				LOG(LOG_ERROR) << "ClientStream::Connect:sendMessage error=" << err;
				m_err_handler(err);
			}
			});
		if (ret > -2) {
			ret = DoLoop(1);
		}
		return ret;
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

		} while (result != psio::Network::IO_PENDING && m_state != HERMES_NONE);
		return result;
	}

	void Stream::CallDoLoop(int result)
	{
		int res = DoLoop(result);
		//if ((res != psio::Network::Err::IO_PENDING) && (m_message_handler != NULL))
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
			if (result != -1 && m_err_handler)
				m_err_handler(result);
			return -1;
		}

		// check if message is received completely
		std::unique_ptr<Message> msg;
		msg.reset(m_msg_factory.getMessage(m_inbuf.get()));
		if (msg) {
			LOG(LOG_DEBUG) << "[" << m_id << "] Message received: " << msg->getOpcode() << " (" << m_msg_factory.getMessageName(msg->getOpcode()) << ") size=" << msg->base.size;
			int res = 0;
			if (m_inbuf->Remaining()) {
				m_next_state = HERMES_STATE::HERMES_READMESSAGE_COMPLETED;
				res = (int)m_inbuf->Remaining();
			}
			else {
				m_next_state = HERMES_STATE::HERMES_READMESSAGE;
				res = 0;
			}
			// filter message
			if (m_msg_factory.filter(msg->getOpcode()))
			{
				// the message handler must not destroy |this|
				// it may set next state to 'close' though
				if (!m_msg_factory.handle(msg.get()))
					LOG(LOG_ERROR) << "Unhandled message with id " << msg->getOpcode() << " (" << m_msg_factory.getMessageName(msg->getOpcode()) << ") received.";
			}
			else {
				LOG(LOG_ERROR) << "Message was filtered";
			}
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
		m_connected = false;
		// this may destroy |this|
		if (m_closed_handler)
			m_closed_handler(0);	
		return psio::Network::IO_PENDING;
	}

	void Stream::onConnect(MessageConnect* msg)
	{
		// check if protocol version is available
		if (msg->protocol_version != HERMES_PROTOCOL_VERSION) {
			// wrong protocol version
			Message res_msg(HERMES_ERR_PROTOCOL_VERSION);
			sendMessage(&res_msg, [&](int err) {
				// note this will probably destroy |this|
				m_err_handler(-HERMES_ERR_PROTOCOL_VERSION);
				});
			return;
		}
		m_connected = true;
		if (m_on_connection_handler)
			m_on_connection_handler(0);

		// connection established
		Message res_msg(HERMES_MSG_CONNECTED);
		sendMessage(&res_msg, nullptr);
	}

	void Stream::onConnectedMessage(Message* msg)
	{
		m_next_state = HERMES_READMESSAGE;
		m_connected = true;
		if (m_on_connection_handler)
			m_on_connection_handler(0);
	}

	void Stream::onErrorMessage(Message* msg)
	{
		if (m_err_handler)
			m_err_handler(msg->base.opcode);
	}
}