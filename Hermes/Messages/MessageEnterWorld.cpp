#include "MessageEnterWorld.h"

namespace Hermes {
	void MessageEnterWorld::serialize(Psio::IOBuffer* buf)
	{

		base.size = sizeof(BaseMessage) + sizeof(uint32_t) * 1 + avatar.length();
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();
		
		size += write(dst + size, avatar);

		buf->Produce(size);
	}
	
	void MessageEnterWorld::unserialize(Psio::IOBuffer* buf)
	{
		avatar = readStr(buf);
	}

	Message* MessageEnterWorld::create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf)
	{
		MessageEnterWorld* msg;
		msg = new MessageEnterWorld();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}
