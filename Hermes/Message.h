#ifndef HERMES_MESSAGE_H
#define HERMES_MESSAGE_H

#include <Psio/Net/IOBuffer.h>
#include <cstdint>
#include <string>

namespace Hermes
{
	class Message
	{
	public:
		Message() :
			Message(0)
		{}
		Message(uint16_t opcode);
		virtual ~Message() {}

		virtual void serialize(Psio::IOBuffer* buf);

		uint16_t getOpcode() { return base.opcode; }

		static Message* create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf);

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

		uint32_t readL(Psio::IOBuffer* buf);
		uint16_t readS(Psio::IOBuffer* buf);
		std::string readStr(Psio::IOBuffer* buf);
		float readF(Psio::IOBuffer* buf);

	};
}

#endif // !HERMES_MESSAGE_H
