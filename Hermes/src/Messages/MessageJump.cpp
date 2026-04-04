#include "Hermes/Messages/MessageJump.h"

namespace hermes
{
	void MessageJump::serialize(psio::IOBuffer* buf)
	{
		base.size = sizeof(BaseMessage) + sizeof(uint32_t) * 1;
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();

		size += write(dst, NUID);

		buf->Produce(size);
	}

	void MessageJump::unserialize(psio::IOBuffer* buf)
	{
		NUID = readL(buf);
	}

	Message* MessageJump::create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf)
	{
		MessageJump* msg;
		msg = new MessageJump();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}

}