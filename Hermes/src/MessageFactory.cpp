#include <Psio/Net/Network.h>
#include "Hermes/MessageFactory.h"

#include "Hermes/Messages/MessageCreate.h"
#include "Hermes/Messages/MessageDestroy.h"
#include "Hermes/Messages/MessageJump.h"
#include "Hermes/Messages/MessageMove.h"
#include "Hermes/Messages/MessagePosition.h"
#include "Hermes/Messages/MessageTurn.h"
#include "Hermes/Messages/MessageEnterWorld.h"
#include "Hermes/Messages/MessageEnterFailed.h"
#include "Hermes/Messages/MessageAvatarList.h"
#include "Hermes/Messages/MessageLogin.h"
#include "Hermes/Messages/MessageLoginFailed.h"
#include "Hermes/Messages/MessageConnect.h"
#include "Hermes/Messages/MessageCreateUser.h"
#include "Hermes/Messages/MessageCreateAvatar.h"

namespace Hermes
{
	void MessageFactory::registerMessageType(uint16_t opcode, std::string name, CreatorFunction creator)
	{
		m_info[opcode].creator = creator;
		m_info[opcode].name = name;
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
				if (m_info[baseH.opcode].creator)
					creator = m_info[baseH.opcode].creator;
				else
					creator = Message::create;
				Message* msg = creator(baseH.opcode, baseH.size, buf);
				return msg;
			}
		}
		// not read yet
		return nullptr;
	}
	
	bool MessageFactory::handle(Message* msg)
	{
		if (m_info[msg->getOpcode()].handler) {
			m_info[msg->getOpcode()].handler(msg);
			return true;
		}
		return false;
	}

	bool MessageFactory::filter(uint16_t opcode)
	{
		if (m_info[opcode].filter)
			return m_info[opcode].filter(opcode);
		return true;
	}

	std::string MessageFactory::getMessageName(uint16_t opcode)
	{
		if (m_info[opcode].name == "")
			return "Unknown";
		else
			return m_info[opcode].name;
	}

	void MessageFactory::registerCoreMessages()
	{
		// connection messages
		registerMessageType(HERMES_MSG_CONNECT, "Connect", MessageConnect::create);

		// auth messages
		registerMessageType(HERMES_MSG_LOGIN, "Login", MessageLogin::create);
		registerMessageType(HERMES_MSG_LOGIN_FAILED, "LoginFailed", MessageLoginFailed::create);
		registerMessageType(HERMES_MSG_CREATE_USER, "CreateUser", MessageCreateUser::create);
		registerMessageType(HERMES_MSG_CREATE_USER_FAILED, "CreateUserFailed", MessageLoginFailed::create);

		// lobby messages
		registerMessageType(HERMES_MSG_ENTERWORLD, "EnterWorld", MessageEnterWorld::create);
		registerMessageType(HERMES_MSG_ENTER_FAILED, "EnterWorldFailed", MessageEnterFailed::create);
		registerMessageType(HERMES_MSG_GET_AVATARLIST, "GetAvatarList", Message::create);
		registerMessageType(HERMES_MSG_AVATARLIST, "AvatarList", MessageAvatarList::create);
		registerMessageType(HERMES_MSG_CREATEAVATAR, "CreateAvatar", MessageCreateAvatar::create);
		registerMessageType(HERMES_MSG_CREATEAVATAR_FAILED, "CreateAvatarFailed", MessageEnterFailed::create);
	
		// world messages
		registerMessageType(HERMES_MSG_CREATE, "Create", MessageCreate::create);
		registerMessageType(HERMES_MSG_DESTROY, "Destroy", MessageDestroy::create);
		
		registerMessageType(HERMES_MSG_POSITION, "Position", MessagePosition::create);
		registerMessageType(HERMES_MSG_MOVE, "Move", MessageMove::create);
		registerMessageType(HERMES_MSG_TURN, "Turn", MessageTurn::create);
		registerMessageType(HERMES_MSG_JUMP, "Jump", MessageJump::create);
	}
}