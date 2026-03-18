#include "Hermes/Messages/MessageCreateUser.h"

namespace Hermes
{
	void MessageCreateUser::serialize(Psio::IOBuffer* buf)
	{
		base.size = (uint16_t)(sizeof(BaseMessage) + 4 * sizeof(uint32_t) + auth_method.length() + user_name.length() + name.length() + ext_id.length() + sizeof(uint16_t));
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
		size += write(dst + size, name);
		size += write(dst + size, ext_id);
		size += write(dst + size, secret_size);

		if (secrets.size() > 0)
			for (size_t i = 0; i < secrets.size(); i++)
				size += write(dst + size, secrets[i]);

		buf->Produce(size);
	}

	void MessageCreateUser::unserialize(Psio::IOBuffer* buf)
	{
		auth_method = readStr(buf);
		user_name = readStr(buf);
		name = readStr(buf);
		ext_id = readStr(buf);
		uint16_t secret_size = readS(buf);
		if (secret_size)
			for (size_t i = 0; i < secret_size; i++)
				secrets.push_back(readStr(buf));
	}

	Message* MessageCreateUser::create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf)
	{
		MessageCreateUser* msg;
		msg = new MessageCreateUser(opcode);

		msg->base.size = size;
		msg->unserialize(buf);
		return msg;
	}
}