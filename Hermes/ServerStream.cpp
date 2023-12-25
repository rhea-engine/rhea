#include <Psio/Net/IOBuffer.h>

#include "ServerStream.h"
#include <Opcode.h>
#include <Psio/Log.h>
#include <Hermes/Messages/MessageLogin.h>
#include <Messages/MessageConnect.h>
#include <Hermes/Messages/MessageLoginFailed.h>
#include <Messages/MessageCreateUser.h>

using Psio::Network;

namespace Hermes
{
	void ServerStream::Initialize()
	{
		MessageFactory* factory = m_stream->getMessageFactory();
		if (!factory) {
			LOG(LOG_ERROR) << "No message factory found";
			return;
		}
		
		factory->registerMessageHandler<MessageLogin>(HERMES_MSG_LOGIN,
			[&](uint16_t opcode)->bool {
				return m_state == HERMES_LOGIN_PENDING;
			},
			std::bind(&ServerStream::onLoginMessage, this, std::placeholders::_1)
		);

		factory->registerMessageHandler<MessageCreateUser>(HERMES_MSG_CREATE_USER,
			[&](uint16_t opcode)->bool {
				return m_state == HERMES_LOGIN_PENDING;
			},
			std::bind(&ServerStream::onCreateMessage, this, std::placeholders::_1)
		);

		m_state = HERMES_LOGIN_PENDING;
	}

	int ServerStream::AuthSuccess(Stream::CompletionHandler handler)
	{
		Hermes::Message msg_auth(HERMES_MSG_LOGIN_SUCCEEDED);
		m_state = HERMES_READMESSAGE;

		int ret = m_stream->sendMessage(&msg_auth, [&, handler](int result) {
			if (handler)
				handler(result);
			});

		if (ret > -2)
			ret = 1;

		return ret;
	}

	int ServerStream::AuthFailure(Stream::CompletionHandler handler, std::vector<std::string> methods)
	{
		Hermes::MessageLoginFailed msg_auth;
		msg_auth.methods = methods;

		return m_stream->sendMessage(&msg_auth, [&, handler](int result) {
			if (handler)
				handler(result);
			});
	}

	void ServerStream::onLoginMessage(MessageLogin* msg)
	{
		if (m_login_handler)
			m_login_handler(msg->auth_method, msg->user_name, msg->secrets);
	}

	void ServerStream::onCreateMessage(MessageCreateUser* msg)
	{
		if (m_create_handler)
			m_create_handler(msg->auth_method, msg->user_name, msg->name, msg->ext_id, msg->secrets);
	}

	int ServerStream::LoginRetry()
	{
		m_state = HERMES_LOGIN_PENDING;
		return -1;
	}
}