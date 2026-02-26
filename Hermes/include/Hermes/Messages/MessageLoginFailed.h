#ifndef HERMES_MESSAGE_LOGINFAILED_H
#define HERMES_MESSAGE_LOGINFAILED_H

#include <Hermes/Message.h>
#include <Hermes/Opcode.h>
#include <vector>

namespace Hermes
{
	class MessageLoginFailed : public Message
	{
	public:
		MessageLoginFailed() :
			Message(HERMES_MSG_LOGIN_FAILED)
		{}

		void serialize(Psio::IOBuffer* buf);
		void unserialize(Psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf);

		std::vector<std::string> methods;
	};
}


#endif // !HERMES_MESSAGE_LOGIN_H
