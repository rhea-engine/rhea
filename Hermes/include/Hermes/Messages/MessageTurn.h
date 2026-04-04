#ifndef HERMES_MESSAGE_TURN_H
#define HERMES_MESSAGE_TURN_H

#include "../Message.h"
#include "../Opcode.h"

namespace hermes
{
	class MessageTurn : public Message
	{
	public:
		MessageTurn() :
			NUID(0),
			turn(0.0f),
			Message(HERMES_MSG_TURN)
		{}

		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		uint32_t NUID;
		float turn;
	};
}

#endif // !HERMES_MESSAGE_MOVE_H