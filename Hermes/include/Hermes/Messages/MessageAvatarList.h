/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HERMES_MESSAGE_AVATARLIST_H
#define HERMES_MESSAGE_AVATARLIST_H

#include "../Message.h"
#include "../Opcode.h"
#include <vector>

namespace hermes
{
	class MessageAvatarList : public Message
	{
	public:
		MessageAvatarList() :
			Message(HERMES_MSG_AVATARLIST)
		{}

		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);
		
		void addAvatar(std::string& avatar_name, std::string& type_name);

		typedef struct {
			std::string avatar;
			std::string type_name;
		} avatardef;
		std::vector<avatardef> avatars;
	};
}

#endif // !HERMES_MESSAGE_ENTERWORLD_H