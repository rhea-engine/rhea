/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HERMES_MESSAGE_JUMP_H
#define HERMES_MESSAGE_JUMP_H

#include "../Message.h"
#include "../Opcode.h"

namespace hermes
{
	class MessageJump : public Message
	{
	public:
		MessageJump() :
			NUID(0),			
			Message(HERMES_MSG_JUMP)
		{}

		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		uint32_t NUID;		
	};
}

#endif // !HERMES_MESSAGE_MOVE_H