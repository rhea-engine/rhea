#include "MessageCreateAvatar.h"

namespace Hermes
{
	void MessageCreateAvatar::serialize(Psio::IOBuffer* buf)
	{
		base.size = (uint16_t)(sizeof(BaseMessage) + 2 * sizeof(uint32_t) + avatar.name.length() + avatar.type_name.length());
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();

		size += write(dst + size, avatar.name);
		size += write(dst + size, avatar.type_name);

		buf->Produce(size);
	}

	void MessageCreateAvatar::unserialize(Psio::IOBuffer* buf)
	{
		avatar.name = readStr(buf);
		avatar.type_name = readStr(buf);
	}

	Message* MessageCreateAvatar::create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf)
	{
		MessageCreateAvatar* msg;
		msg = new MessageCreateAvatar(opcode);

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}