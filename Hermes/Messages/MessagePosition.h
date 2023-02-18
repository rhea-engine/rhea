#ifndef HERMES_MESSAGEPOSITION_H
#define HERMES_MESSAGE_POSITION_H

#include "../Message.h"
#include "../Opcode.h"
#include <string>

namespace Hermes
{
	class MessagePosition : public Message
	{
	public:
		MessagePosition() :
			NUID(0),			
			pos{ 0.0f, 0.0f, 0.0f },
			transform{ 0.0f, 0.0f, 0.0f },
			Message(HERMES_MSG_POSITION)
		{}

		void serialize(Psio::IOBuffer* buf);
		void unserialize(Psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf);

		void setPosition(float x, float y, float z);
		void setTransform(float w, float x, float y, float z);

		uint32_t NUID;
		float pos[3];
		float transform[4];		
	};
}

#endif // !HERMES_MESSAGE_CREATE_H
