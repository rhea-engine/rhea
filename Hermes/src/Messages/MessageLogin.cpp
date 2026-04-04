#include "Hermes/Messages/MessageLogin.h"

namespace hermes
{
	void MessageLogin::serialize(psio::IOBuffer* buf)
	{
		base.size = sizeof(BaseMessage) + 2 * sizeof(uint32_t) + (uint16_t)auth_method.length() + (uint16_t)user_name.length() + sizeof(uint16_t);
		if (secrets.size() > 0) {
			for (size_t i = 0; i < secrets.size(); i++)
			{
				base.size += sizeof(uint32_t);
				base.size += (uint16_t)secrets[i].length();
			}
		}
		if (buf->FreeBufSize() < base.size) {
			buf->Grow(base.size);
		}
		Message::serialize(buf);

		int size = 0;
		char* dst = buf->writer();
		uint16_t secret_size = (uint16_t)secrets.size();

		size += write(dst + size, auth_method);
		size += write(dst + size, user_name);
		size += write(dst + size, secret_size);
		if (secrets.size() > 0)
			for (size_t i = 0; i < secrets.size(); i++)
				size += write(dst + size, secrets[i]);

		buf->Produce(size);
	}

	void MessageLogin::unserialize(psio::IOBuffer* buf)
	{
		auth_method = readStr(buf);
		user_name = readStr(buf);
		uint16_t secret_size = readS(buf);
		if (secret_size)
			for (size_t i = 0; i < secret_size; i++)
				secrets.push_back(readStr(buf));
	}

	Message* MessageLogin::create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf)
	{
		MessageLogin* msg;
		msg = new MessageLogin(opcode);

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}