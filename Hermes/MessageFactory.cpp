#include <Psio/Net/Network.h>
#include "MessageFactory.h"

#include "Messages/MessageCreate.h"
#include "Messages/MessageDestroy.h"
#include "Messages/MessageJump.h"
#include "Messages/MessageMove.h"
#include "Messages/MessagePosition.h"
#include "Messages/MessageTurn.h"
#include "Messages/MessageEnterWorld.h"
#include "Messages/MessageEnterFailed.h"
#include "Messages/MessageAvatarList.h"
#include "Messages/MessageLogin.h"
#include "Messages/MessageLoginFailed.h"
#include "Messages/MessageConnect.h"

namespace Hermes
{
	void MessageFactory::registerMessageType(uint16_t opcode, CreatorFunction creator)
	{
		m_creators[opcode] = creator;
	}
	
	Message* MessageFactory::getMessage(Psio::IOBuffer* buf)
	{
		// check if message is received completely
		if (buf->Remaining() >= sizeof(Message::BaseMessage)) {
			Message::BaseMessage* basebuf = (Message::BaseMessage*)(buf->reader());
			Message::BaseMessage baseH;
			baseH.opcode = ntohs(basebuf->opcode);
			baseH.size = ntohs(basebuf->size);
			if (buf->Remaining() >= baseH.size) {
				// yes
				buf->Consume(sizeof(Message::BaseMessage));

				// read the message
				// get the creator for the opcode
				CreatorFunction creator;
				if (m_creators.find(baseH.opcode) == m_creators.end())
					creator = Message::create;
				else
					creator = m_creators[baseH.opcode];
				Message* msg = creator(baseH.opcode, baseH.size, buf);
				return msg;
			}
		}
		// not read yet
		return nullptr;
	}
	
	void MessageFactory::registerCoreMessages()
	{
		// connection messages
		registerMessageType(HERMES_MSG_CONNECT, MessageConnect::create);

		// auth messages
		registerMessageType(HERMES_MSG_LOGIN, MessageLogin::create);
		registerMessageType(HERMES_MSG_LOGIN_FAILED, MessageLoginFailed::create);

		// lobby messages
		registerMessageType(HERMES_MSG_ENTERWORLD, MessageEnterWorld::create);
		registerMessageType(HERMES_MSG_ENTER_FAILED, MessageEnterFailed::create);
		registerMessageType(HERMES_MSG_AVATARLIST, MessageAvatarList::create);

		// world messages
		registerMessageType(HERMES_MSG_CREATE, MessageCreate::create);
		registerMessageType(HERMES_MSG_DESTROY, MessageDestroy::create);
		
		registerMessageType(HERMES_MSG_POSITION, MessagePosition::create);
		registerMessageType(HERMES_MSG_MOVE, MessageMove::create);
		registerMessageType(HERMES_MSG_TURN, MessageTurn::create);
		registerMessageType(HERMES_MSG_JUMP, MessageJump::create);
	}
}