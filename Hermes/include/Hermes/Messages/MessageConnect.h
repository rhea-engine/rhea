/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HERMES_MESSAGE_CONNECT
#define HERMES_MESSAGE_CONNECT

#include <Hermes/Message.h>
#include <Hermes/Opcode.h>

namespace hermes
{
	class MessageConnect : public Message
	{
	public:
		MessageConnect() :
			protocol_version(HERMES_PROTOCOL_VERSION),
			Message(HERMES_MSG_CONNECT)
		{}

		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		uint32_t protocol_version;
		std::string world_name;
	};
}

#endif // HERMES_MESSAGE_CONNECT