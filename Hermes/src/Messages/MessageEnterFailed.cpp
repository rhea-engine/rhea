#include "Hermes/Messages/MessageEnterFailed.h"

namespace hermes {
	void MessageEnterFailed::serialize(psio::IOBuffer* buf)
	{
		base.size = sizeof(BaseMessage) + sizeof(reason);
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();

		size += write(dst + size, reason);

		buf->Produce(size);
	}

	void MessageEnterFailed::unserialize(psio::IOBuffer* buf)
	{
		reason = readS(buf);
	}

	Message* MessageEnterFailed::create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf)
	{
		MessageEnterFailed* msg;
		msg = new MessageEnterFailed();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}
