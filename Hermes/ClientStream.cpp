#include "ClientStream.h"
#include <Psio/Log.h>
#include <Opcode.h>
#include <Messages/MessageLogin.h>
#include <Messages/MessageConnect.h>
#include <Messages/MessageCreateUser.h>

namespace Hermes
{

	void ClientStream::Initialize()
	{
		MessageFactory* factory = m_stream->getMessageFactory();
		if (!factory) {
			LOG(LOG_ERROR) << "No message factory found";
			return;
		}
		
		// login handling
		factory->registerMessageHandler<Message>(HERMES_MSG_LOGIN_SUCCEEDED,
			[&](uint16_t opcode)->bool {
				return m_state == HERMES_LOGIN_PENDING;
			},
			std::bind(&ClientStream::onLoginMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<MessageLoginFailed>(HERMES_MSG_LOGIN_FAILED,
			[&](uint16_t opcode)->bool {
				return m_state == HERMES_LOGIN_PENDING;
			},
			std::bind(&ClientStream::onLoginFailureMessage, this, std::placeholders::_1)
		); 
			
		factory->registerMessageHandler<MessageLoginFailed>(HERMES_MSG_CREATE_USER_FAILED,
				[&](uint16_t opcode)->bool {
					return m_state == HERMES_LOGIN_PENDING;
				},
				std::bind(&ClientStream::onLoginFailureMessage, this, std::placeholders::_1)
			);
	}

	int ClientStream::Loop()
	{
		m_state = HERMES_READMESSAGE;
		return -1;
	}

	int ClientStream::Login(std::string method, std::string username, std::vector<std::string> secrets, LoginHandler handler)
	{
		m_login_handler = handler;

		if (!handler) {
			LOG(LOG_ERROR) << "ClientStream::Login: No login handler provided";
			return -2;
		}

		Hermes::MessageLogin msg;
		msg.auth_method = method;
		msg.user_name = username;
		msg.secrets = secrets;
		m_state = HERMES_LOGIN_PENDING;

		return m_stream->sendMessage((Message*)&msg, nullptr);
	}

	int ClientStream::Register(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets, LoginHandler handler)
	{
		m_login_handler = handler;

		if (!handler) {
			LOG(LOG_ERROR) << "ClientStream::Login: No login handler provided";
			return -2;
		}

		Hermes::MessageCreateUser msg(HERMES_MSG_CREATE_USER);
		msg.auth_method = method;
		msg.user_name = username;
		if (name != "")
			msg.name = name;
		else
			msg.name = username;
		msg.ext_id = ext_id;
		msg.secrets = secrets;
		m_state = HERMES_LOGIN_PENDING;

		return m_stream->sendMessage((Message*)&msg, nullptr);
	}

	void ClientStream::onLoginMessage(Message* msg)
	{
		m_state = HERMES_READMESSAGE;
		if (m_login_handler)
			m_login_handler(0);
	}

	void ClientStream::onLoginFailureMessage(MessageLoginFailed* msg)
	{
		if (m_login_handler)
			m_login_handler(msg->base.opcode);
	}
}