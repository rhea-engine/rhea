/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HERMES_MESSAGE_CREATEUSER_H
#define HERMES_MESSAGE_CREATEUSER_H

#include <Hermes/Message.h>
#include <Hermes/Opcode.h>
#include <vector>

namespace hermes
{
	class MessageCreateUser : public Message
	{
	public:
		MessageCreateUser() :
			MessageCreateUser(HERMES_MSG_CREATE_USER)
		{}
		MessageCreateUser(uint16_t opcode) :
			Message(opcode)
		{}

		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		std::string auth_method;
		std::string user_name;
		std::string name;
		std::string ext_id;
		std::vector<std::string> secrets;
	};
}


#endif // !HERMES_MESSAGE_CREATEUSER_H
