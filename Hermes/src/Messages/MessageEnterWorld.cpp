/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Hermes/Messages/MessageEnterWorld.h"

namespace hermes {
	void MessageEnterWorld::serialize(psio::IOBuffer* buf)
	{

		base.size = sizeof(BaseMessage) + sizeof(uint32_t) * 1 + (uint16_t)avatar.length();
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();
		
		size += write(dst + size, avatar);

		buf->Produce(size);
	}
	
	void MessageEnterWorld::unserialize(psio::IOBuffer* buf)
	{
		avatar = readStr(buf);
	}

	Message* MessageEnterWorld::create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf)
	{
		MessageEnterWorld* msg;
		msg = new MessageEnterWorld();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}
