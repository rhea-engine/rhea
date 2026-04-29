/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Hermes/Message.h"
#include <Psio/Net/Network.h>
#include <cstring>

namespace hermes
{
	Message::Message(uint16_t opcode)
	{
		base.opcode = opcode;
		base.size = sizeof(BaseMessage);
	}

	void Message::serialize(psio::IOBuffer* buf)
	{
		if (buf->FreeBufSize() < sizeof(BaseMessage)) {
			buf->Grow(sizeof(BaseMessage));
		}
		int size = write(buf->writer(), base.size);
		size += write(buf->writer() + size, base.opcode);
		buf->Produce(size);
	}
	
	Message* Message::create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf)
	{
		Message* msg = new Message(opcode);
		msg->base.size = size;
		return msg;
	}

	int Message::write(char* buf, uint32_t l)
	{
		*(uint32_t*)(&buf[0]) = htonl(l);
		return 4;
	}

	int Message::write(char* buf, uint16_t s)
	{
		*(uint16_t*)(&buf[0]) = htons(s);
		return 2;
	}

	int Message::write(char* buf, std::string str)
	{
		*(uint32_t*)(&buf[0]) = htonl((uint32_t)str.size());
		int size = 4;
		memcpy(&buf[size], str.c_str(), str.size());
		size += (int)str.size();
		return size;
	}

	int Message::write(char* buf, float f)
	{
		char c[4];
		*(float*)(&c[0]) = f;
		*(uint32_t*)(&buf[0]) = htonl(*(uint32_t*)(&c[0]));
		return 4;
	}

	uint32_t Message::readL(psio::IOBuffer* buf)
	{
		if (buf->Remaining() < 4)
			return 0;
		uint32_t l = ntohl(*(uint32_t*)(buf->reader()));
		buf->Consume(4);
		return l;
	}
	
	uint16_t Message::readS(psio::IOBuffer* buf)
	{
		if (buf->Remaining() < 2)
			return 0;
		uint16_t s = ntohs(*(uint16_t*)(buf->reader()));
		buf->Consume(2);
		return s;
	}
	
	std::string Message::readStr(psio::IOBuffer* buf)
	{
		uint32_t strlen = readL(buf);
		if (!strlen || buf->Remaining() < strlen)
			return "";
		std::string str = std::string(buf->reader(), strlen);		
		buf->Consume(strlen);
		return str;
	}
	
	float Message::readF(psio::IOBuffer* buf)
	{
		if (buf->Remaining() < 4)
			return 0;
		uint32_t l = ntohl(*(uint32_t*)(buf->reader()));
		buf->Consume(4);
		float f = *(float*)(&l); 
		return f;
	}
}