#include "Hermes/Messages/MessageCreate.h"
#include <Psio/Net/Network.h>

namespace Hermes
{
	void MessageCreate::serialize(Psio::IOBuffer* buf)
	{
		base.size = sizeof(BaseMessage) + sizeof(uint32_t) * 10 + sizeof(uint16_t) + (uint16_t)path.size() + (uint16_t)type_name.size();
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();

		size += write(dst, NUID);
		size += write(dst + size, path);
		size += write(dst + size, type_name);
		size += write(dst + size, pos[0]);
		size += write(dst + size, pos[1]);
		size += write(dst + size, pos[2]);
		size += write(dst + size, transform[0]);
		size += write(dst + size, transform[1]);
		size += write(dst + size, transform[2]);
		size += write(dst + size, transform[3]);
		size += write(dst + size, (uint16_t)is_player);
		buf->Produce(size);
	}

	void MessageCreate::unserialize(Psio::IOBuffer* buf)
	{
		NUID = readL(buf);
		path = readStr(buf);
		type_name = readStr(buf);
		pos[0] = readF(buf);
		pos[1] = readF(buf);
		pos[2] = readF(buf);
		transform[0] = readF(buf);
		transform[1] = readF(buf);
		transform[2] = readF(buf);
		transform[3] = readF(buf);
		is_player = (bool)readS(buf);
	}

	Message* MessageCreate::create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf)
	{
		MessageCreate* msg;
		msg = new MessageCreate();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}