/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Hermes/Messages/MessageMove.h"

namespace hermes
{

	void MessageMove::serialize(psio::IOBuffer* buf)
	{
		base.size = sizeof(BaseMessage) + sizeof(uint32_t) * 4;
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();

		size += write(dst, NUID);
		size += write(dst + size, move[0]);
		size += write(dst + size, move[1]);
		size += write(dst + size, move[2]);

		buf->Produce(size);
	}

	void MessageMove::unserialize(psio::IOBuffer* buf)
	{
		NUID = readL(buf);
		move[0] = readF(buf);
		move[1] = readF(buf);
		move[2] = readF(buf);
	}

	Message* MessageMove::create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf)
	{
		MessageMove* msg;
		msg = new MessageMove();

		msg->base.size = size;
		msg->unserialize(buf);
	
		return msg;
	}
}