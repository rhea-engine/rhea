#include "MessageLogin.h"

namespace Hermes
{
	void MessageLogin::serialize(Psio::IOBuffer* buf)
	{
		base.size = sizeof(BaseMessage) + sizeof(uint32_t) + sizeof(uint32_t) + (uint16_t)user_name.length();
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();

		size += write(dst + size, protocol_version);
		size += write(dst + size, user_name);

		buf->Produce(size);
	}

	void MessageLogin::unserialize(Psio::IOBuffer* buf)
	{
		protocol_version = readL(buf);
		user_name = readStr(buf);
	}

	Message* MessageLogin::create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf)
	{
		MessageLogin* msg;
		msg = new MessageLogin();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}