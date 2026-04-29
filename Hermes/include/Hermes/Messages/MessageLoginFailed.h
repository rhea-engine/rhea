/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HERMES_MESSAGE_LOGINFAILED_H
#define HERMES_MESSAGE_LOGINFAILED_H

#include <Hermes/Message.h>
#include <Hermes/Opcode.h>
#include <vector>

namespace hermes
{
	class MessageLoginFailed : public Message
	{
	public:
		MessageLoginFailed() :
			Message(HERMES_MSG_LOGIN_FAILED)
		{}

		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		std::vector<std::string> methods;
	};
}


#endif // !HERMES_MESSAGE_LOGIN_H
