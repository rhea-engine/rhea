#ifndef HERMES_MESSAGE_ENTERWORLD_H
#define HERMES_MESSAGE_ENTERWORLD_H

#include "../Message.h"
#include "../Opcode.h"

namespace Hermes
{
	class MessageEnterWorld : public Message
	{
	public:
		MessageEnterWorld() :
			Message(HERMES_MSG_ENTERWORLD)
		{}

		void serialize(Psio::IOBuffer* buf);
		void unserialize(Psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf);

		std::string avatar;
	};
}

#endif // !HERMES_MESSAGE_ENTERWORLD_H