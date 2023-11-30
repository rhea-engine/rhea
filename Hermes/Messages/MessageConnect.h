#ifndef HERMES_MESSAGE_CONNECT
#define HERMES_MESSAGE_CONNECT

#include "Message.h"
#include <Opcode.h>

namespace Hermes
{
	class MessageConnect : public Message
	{
	public:
		MessageConnect() :
			protocol_version(HERMES_PROTOCOL_VERSION),
			Message(HERMES_MSG_CONNECT)
		{}

		void serialize(Psio::IOBuffer* buf);
		void unserialize(Psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf);

		uint32_t protocol_version;
		std::string world_name;
	};
}

#endif // HERMES_MESSAGE_CONNECT