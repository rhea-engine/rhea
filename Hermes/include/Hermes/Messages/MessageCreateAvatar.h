/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HERMES_MESSAGE_CREATEAVATAR_H
#define HERMES_MESSAGE_CREATEAVATAR_H

#include "../Message.h"
#include "../Opcode.h"

namespace hermes
{
	class MessageCreateAvatar : public Message
	{
	public:
		MessageCreateAvatar() :
			MessageCreateAvatar(HERMES_MSG_CREATEAVATAR)
		{}
		MessageCreateAvatar(uint16_t opcode) :
			Message(opcode)
		{}

		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		typedef struct {
			std::string name;
			std::string type_name;
		} avatardef;
		avatardef avatar;
	};
}

#endif // !HERMES_MESSAGE_CREATEAVATAR_H