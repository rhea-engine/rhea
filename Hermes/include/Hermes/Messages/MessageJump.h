#ifndef HERMES_MESSAGE_JUMP_H
#define HERMES_MESSAGE_JUMP_H

#include "../Message.h"
#include "../Opcode.h"

namespace Hermes
{
	class MessageJump : public Message
	{
	public:
		MessageJump() :
			NUID(0),			
			Message(HERMES_MSG_JUMP)
		{}

		void serialize(Psio::IOBuffer* buf);
		void unserialize(Psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf);

		uint32_t NUID;		
	};
}

#endif // !HERMES_MESSAGE_MOVE_H