/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HERMES_MESSAGEPOSITION_H
#define HERMES_MESSAGEPOSITION_H

#include "../Message.h"
#include "../Opcode.h"
#include <string>

namespace hermes
{
	class MessagePosition : public Message
	{
	public:
		MessagePosition() :
			NUID(0),			
			pos{ 0.0f, 0.0f, 0.0f },
			transform{ 0.0f, 0.0f, 0.0f },
			Message(HERMES_MSG_POSITION)
		{}

		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		void setPosition(float x, float y, float z);
		void setTransform(float w, float x, float y, float z);

		uint32_t NUID;
		float pos[3];
		float transform[4];		
	};
}

#endif // !HERMES_MESSAGEPOSITION_H
