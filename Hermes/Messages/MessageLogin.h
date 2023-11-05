#ifndef HERMES_MESSAGE_LOGIN_H
#define HERMES_MESSAGE_LOGIN_H

#include <Hermes/Message.h>
#include <Hermes/Opcode.h>

namespace Hermes
{
	class MessageLogin : public Message
	{
	public:
		MessageLogin() :
			protocol_version(HERMES_PROTOCOL_VERSION),
			Message(HERMES_MSG_LOGIN)
		{}

		void serialize(Psio::IOBuffer* buf);
		void unserialize(Psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf);

		uint32_t protocol_version;
		std::string world_name;
		std::string user_name;
	};
}


#endif // !HERMES_MESSAGE_LOGIN_H
