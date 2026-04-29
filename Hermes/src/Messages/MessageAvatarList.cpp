/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Hermes/Messages/MessageAvatarList.h"
#include <Psio/Net/Network.h>

namespace hermes {
	void MessageAvatarList::serialize(psio::IOBuffer* buf)
	{
		BaseMessage* bmsg = (BaseMessage*)buf->writer();
		uint16_t nof_avatars = (uint16_t)avatars.size();
		base.size = sizeof(BaseMessage) + sizeof(uint16_t) * 1 + nof_avatars * 160; // this is only an estimate for a large enough buffer
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();

		size += write(dst + size, nof_avatars);
		for (auto it = avatars.begin(); it != avatars.end(); ++it) {
			size += write(dst + size, (*it).avatar);
			size += write(dst + size, (*it).type_name);
		}
		buf->Produce(size);
		bmsg->size = htons(sizeof(BaseMessage) + (uint16_t)size);
	}

	void MessageAvatarList::unserialize(psio::IOBuffer* buf)
	{
		uint16_t nof_avatars = readS(buf);
		for (int i = 0; i < nof_avatars; i++) {
			avatardef avatar;
			avatar.avatar = readStr(buf);
			avatar.type_name = readStr(buf);
			avatars.push_back(avatar);
		}
	}

	Message* MessageAvatarList::create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf)
	{
		MessageAvatarList* msg;
		msg = new MessageAvatarList();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}

	void MessageAvatarList::addAvatar(std::string& avatar_name, std::string& type_name)
	{
		avatardef avatar;
		avatar.avatar = avatar_name;
		avatar.type_name = type_name;
		avatars.push_back(avatar);
	}
}
