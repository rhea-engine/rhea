#ifndef HERMES_MESSAGE_DESTROY_H
#define HERMES_MESSAGE_DESTROY_H

#include "../Message.h"
#include "../Opcode.h"

namespace Hermes
{
	class MessageDestroy : public Message
	{
	public:
		MessageDestroy() :
			NUID(0),
			Message(HERMES_MSG_DESTROY)
		{}

		void serialize(Psio::IOBuffer* buf);
		void unserialize(Psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf);

		uint32_t NUID;
	};
}

#endif // !HERMES_MESSAGE_DESTROY_H