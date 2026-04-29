/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HERMES_MESSAGE_MOVE_H
#define HERMES_MESSAGE_MOVE_H

#include "../Message.h"
#include "../Opcode.h"

namespace hermes
{
	class MessageMove : public Message
	{
	public:
		MessageMove() :
			NUID(0),
			move{ 0.0f, 0.0f, 0.0f },
			Message(HERMES_MSG_MOVE)
		{}

		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		uint32_t NUID;
		float move[3];
	};
}

#endif // !HERMES_MESSAGE_MOVE_H