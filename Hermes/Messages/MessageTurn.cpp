#include "MessageTurn.h"

namespace Hermes
{

	void MessageTurn::serialize(Psio::IOBuffer* buf)
	{
		base.size = sizeof(BaseMessage) + sizeof(uint32_t) * 2;
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();

		size += write(dst, NUID);
		size += write(dst + size, turn);

		buf->Produce(size);
	}

	void MessageTurn::unserialize(Psio::IOBuffer* buf)
	{
		NUID = readL(buf);
		turn = readF(buf);
	}

	Message* MessageTurn::create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf)
	{
		MessageTurn* msg;
		msg = new MessageTurn();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}