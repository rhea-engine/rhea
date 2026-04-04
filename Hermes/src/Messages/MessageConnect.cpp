#include "Hermes/Messages/MessageConnect.h"

namespace hermes
{
	void MessageConnect::serialize(psio::IOBuffer* buf)
	{
		base.size = sizeof(BaseMessage) + sizeof(uint32_t) + sizeof(uint32_t) + (uint16_t)world_name.length();
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();

		size += write(dst + size, protocol_version);
		size += write(dst + size, world_name);

		buf->Produce(size);
	}

	void MessageConnect::unserialize(psio::IOBuffer* buf)
	{
		protocol_version = readL(buf);
		world_name = readStr(buf);
	}

	Message* MessageConnect::create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf)
	{
		MessageConnect* msg;
		msg = new MessageConnect();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}