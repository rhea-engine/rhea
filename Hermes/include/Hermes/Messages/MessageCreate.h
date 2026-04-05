#ifndef HERMES_MESSAGE_CREATE_H
#define HERMES_MESSAGE_CREATE_H

#include "../Message.h"
#include "../Opcode.h"
#include <string>

namespace hermes
{
	class MessageCreate : public Message
	{
	public:
		MessageCreate() :
			NUID(0),
			is_player(false),
			pos{ 0.0f, 0.0f, 0.0f },
			transform{ 0.0f, 0.0f, 0.0f, 0.0f },
			Message(HERMES_MSG_CREATE)
		{}
	
		void serialize(psio::IOBuffer* buf);
		void unserialize(psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, psio::IOBuffer* buf);

		uint32_t NUID;
		std::string path;
		std::string type_name;
		float pos[3];
		float transform[4];
		bool is_player;
	};
}

#endif // !HERMES_MESSAGE_CREATE_H
