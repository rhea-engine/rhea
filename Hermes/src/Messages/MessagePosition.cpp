#include "Hermes/Messages/MessagePosition.h"

namespace hermes
{
	void MessagePosition::serialize(psio::IOBuffer* buf)
	{
		base.size = sizeof(BaseMessage) + sizeof(uint32_t) * 8;
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();

		size += write(dst, NUID);
		size += write(dst + size, pos[0]);
		size += write(dst + size, pos[1]);
		size += write(dst + size, pos[2]);
		size += write(dst + size, transform[0]);
		size += write(dst + size, transform[1]);
		size += write(dst + size, transform[2]);
		size += write(dst + size, transform[3]);

		buf->Produce(size);
	}

	void MessagePosition::unserialize(psio::IOBuffer* buf)
	{
		NUID = readL(buf);
		pos[0] = readF(buf);
		pos[1] = readF(buf);
		pos[2] = readF(buf);
		transform[0] = readF(buf);
		transform[1] = readF(buf);
		transform[2] = readF(buf);
		transform[3] = readF(buf);
	}
	
	Message* MessagePosition::create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf)
	{
		MessagePosition* msg;
		msg = new MessagePosition();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}

	void MessagePosition::setPosition(float x, float y, float z)
	{
		pos[0] = x;
		pos[1] = y;
		pos[2] = z;
	}
	
	void MessagePosition::setTransform(float w, float x, float y, float z)
	{
		transform[0] = w;
		transform[1] = x;
		transform[2] = y;
		transform[3] = z;
	}
}
