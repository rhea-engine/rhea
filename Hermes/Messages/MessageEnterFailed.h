#ifndef HERMES_MESSAGE_ENTERFAILED_H
#define HERMES_MESSAGE_ENTERFAILED_H

#include "../Message.h"
#include "../Opcode.h"

namespace Hermes
{
	class MessageEnterFailed : public Message
	{
	public:
		MessageEnterFailed() :
			MessageEnterFailed(HERMES_MSG_ENTER_FAILED)
		{}
		MessageEnterFailed(uint16_t opcode) :
			reason(0),
			Message(opcode)
		{}

		void serialize(Psio::IOBuffer* buf);
		void unserialize(Psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf);

		uint16_t reason;
	};
}

#endif // !HERMES_MESSAGE_ENTERFAILED_H