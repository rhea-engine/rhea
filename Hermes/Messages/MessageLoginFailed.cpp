#include "MessageLoginFailed.h"

namespace Hermes
{
	void MessageLoginFailed::serialize(Psio::IOBuffer* buf)
	{
		base.size = sizeof(BaseMessage) + sizeof(uint16_t);
		if (methods.size() > 0) {
			for (size_t i = 0; i < methods.size(); i++)
			{
				base.size += sizeof(uint32_t);
				base.size += (uint16_t)methods[i].length();
			}
		}
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();
		uint16_t methods_size = (uint16_t)methods.size();

		size += write(dst + size, methods_size);
		if (methods.size() > 0)
			for (size_t i = 0; i < methods.size(); i++)
				size += write(dst + size, methods[i]);

		buf->Produce(size);
	}

	void MessageLoginFailed::unserialize(Psio::IOBuffer* buf)
	{
		uint16_t methods_size = readS(buf);
		if (methods_size)
			for (size_t i = 0; i < methods_size; i++)
				methods.push_back(readStr(buf));
	}

	Message* MessageLoginFailed::create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf)
	{
		MessageLoginFailed* msg;
		msg = new MessageLoginFailed();

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}
