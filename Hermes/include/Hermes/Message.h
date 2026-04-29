/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HERMES_MESSAGE_H
#define HERMES_MESSAGE_H

#include <Psio/Net/IOBuffer.h>
#include <cstdint>
#include <string>

namespace hermes
{
	class Message
	{
	public:
		Message() :
			Message(0)
		{}
		Message(uint16_t opcode);
		virtual ~Message() {}

		virtual void serialize(psio::IOBuffer* buf);

		uint16_t getOpcode() { return base.opcode; }

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		uint16_t getSize() { return base.size; }

		typedef struct
		{
			uint16_t size;
			uint16_t opcode;
		} BaseMessage;

		BaseMessage base;

	protected:

		int write(char* buf, uint32_t l);
		int write(char* buf, uint16_t s);
		int write(char* buf, std::string str);
		int write(char* buf, float f);

		uint32_t readL(psio::IOBuffer* buf);
		uint16_t readS(psio::IOBuffer* buf);
		std::string readStr(psio::IOBuffer* buf);
		float readF(psio::IOBuffer* buf);

	};
}

#endif // !HERMES_MESSAGE_H
