#ifndef HERMES_MESSAGE_AVATARLIST_H
#define HERMES_MESSAGE_AVATARLIST_H

#include "../Message.h"
#include "../Opcode.h"
#include <vector>

namespace Hermes
{
	class MessageAvatarList : public Message
	{
	public:
		MessageAvatarList() :
			Message(HERMES_MSG_AVATARLIST)
		{}

		void serialize(Psio::IOBuffer* buf);
		void unserialize(Psio::IOBuffer* buf);

		static Message* create(uint16_t opcode, uint16_t size, Psio::IOBuffer* buf);
		
		void addAvatar(std::string& avatar_name, std::string& type_name);

		typedef struct {
			std::string avatar;
			std::string type_name;
		} avatardef;
		std::vector<avatardef> avatars;
	};
}

#endif // !HERMES_MESSAGE_ENTERWORLD_H