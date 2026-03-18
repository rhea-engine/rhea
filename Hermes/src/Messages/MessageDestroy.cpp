#include "Hermes/Messages/MessageDestroy.h"

namespace Hermes
{
	void MessageDestroy::serialize(Psio::IOBuffer* buf)
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

	void MessageDestroy::unserialize(Psio::IOBuffer* buf)
	{
		NUID = readL(buf);
	}

	Message* MessageDestroy::create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf)
	{
		MessageDestroy* msg;
		msg = new MessageDestroy();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}

}