#ifndef HERMES_MESSAGE_LOGIN_H
#define HERMES_MESSAGE_LOGIN_H

#include <Hermes/Message.h>
#include <Hermes/Opcode.h>
#include <vector>

namespace hermes
{
	class MessageLogin : public Message
	{
	public:
		MessageLogin() :
			MessageLogin(HERMES_MSG_LOGIN)
		{}
		MessageLogin(uint16_t opcode) :
			Message(opcode)
		{}

		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		std::string auth_method;
		std::string user_name;
		std::vector<std::string> secrets;
	};
}


#endif // !HERMES_MESSAGE_LOGIN_H
