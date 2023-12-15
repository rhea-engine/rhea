#include "ClientStream.h"
#include <Psio/Log.h>
#include <Opcode.h>
#include <Messages/MessageLogin.h>
#include <Messages/MessageConnect.h>
#include <Messages/MessageCreateUser.h>
#include <Messages/MessageCreateAvatar.h>

namespace Hermes
{

	int ClientStream::Connect(std::string world_name, CompletionHandler handler)
	{
		m_inbuf = std::make_unique<Psio::IOBuffer>(64 * 1024);
		m_outbuf = std::make_unique<Psio::IOBuffer>(64 * 1024);

		m_oncompletion_handler = handler;

		Hermes::MessageConnect msg;
		msg.world_name = world_name;
		m_next_state = HERMES_CONNECT_PENDING;

		int ret = sendMessage((Message*)&msg, [&](int err) {
			LOG(LOG_DEBUG) << "ClientStream::Connect:sendMessage " << err;
			CallDoLoop(err);
			});
		if (ret >= msg.getSize()) {
			// synchronous sending - enter loop
			//ret = DoLoop(ret);
			DLOG(LOG_DEBUG) << "sync sendMessage";
		}

		return ret;
	}

	void ClientStream::Login(std::string method, std::string username, std::vector<std::string> secrets, LoginHandler handler)
	{
		m_login_handler = handler;

		if (!handler) {
			LOG(LOG_ERROR) << "ClientStream::Login: No login handler provided";
			return;
		}

		Hermes::MessageLogin msg;
		msg.auth_method = method;
		msg.user_name = username;
		msg.secrets = secrets;
		m_next_state = HERMES_LOGIN_PENDING;

		int ret = sendMessage((Message*)&msg, [&](int err) {
			CallDoLoop(err);
			});
		if (ret >= msg.getSize()) {
			// synchronous sending - enter loop
			DoLoop(ret);
		}
	}

	void ClientStream::Register(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets, LoginHandler handler)
	{
		m_login_handler = handler;

		if (!handler) {
			LOG(LOG_ERROR) << "ClientStream::Login: No login handler provided";
			return;
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
		m_next_state = HERMES_LOGIN_PENDING;

		int ret = sendMessage((Message*)&msg, [&](int err) {
			CallDoLoop(err);
			});
		if (ret >= msg.getSize()) {
			// synchronous sending - enter loop
			DoLoop(ret);
		}
	}

	void ClientStream::CreateAvatar(std::string name, std::string type_name)
	{
		Hermes::MessageCreateAvatar msg(HERMES_MSG_CREATEAVATAR);
		msg.avatar.name = name;
		msg.avatar.type_name = type_name;

		m_next_state = HERMES_READMESSAGE;

		int ret = sendMessage((Message*)&msg, nullptr);
	}

	int ClientStream::onMessage(MessageHandler handler)
	{
		m_message_handler = handler;

		m_next_state = HERMES_READMESSAGE;

		int ret = DoLoop(0);

		return ret;
	}

	void ClientStream::onError(CompletionHandler handler)
	{
		m_err_handler = handler;
	}
	
	int ClientStream::DoLoop(int result)
	{
		do
		{
			m_state = m_next_state;
			m_next_state = HERMES_NONE;

			switch (m_state)
			{
			case HERMES_CONNECT_PENDING:
				result = DoConnectPending(result);
				break;
			case HERMES_CONNECT_COMPLETED:
				result = DoConnectCompleted(result);
				break;
			case HERMES_LOGIN_PENDING:
				result = DoLoginPending(result);
				break;
			case HERMES_LOGIN_COMPLETED:
				result = DoLoginCompleted(result);
				break;
			case HERMES_READMESSAGE:
				result = DoReadMessage(result);
				break;
			case HERMES_READMESSAGE_COMPLETED:
				result = DoReadMessageCompleted(result);
			}

		} while (result != Psio::Network::IO_PENDING && m_state != HERMES_NONE);

		return result;
	}

	void ClientStream::CallDoLoop(int result)
	{
		int res = DoLoop(result);
		if (res != Psio::Network::Err::IO_PENDING)
			DLOG(LOG_DEBUG) << "ClientStream::CallDoLoop: " << res;
		//	if ((res != Network::Err::IO_PENDING) && (m_message_handler != NULL))
		//		m_message_handler(res, result);
	}

	int ClientStream::DoConnectPending(int result)
	{
		if (result < 0)
			return result;

		m_next_state = HERMES_STATE::HERMES_CONNECT_COMPLETED;

		return m_sock->Read(m_inbuf.get(), [&](int result) -> int {
			CallDoLoop(result);
			return -1;
			});
	}

	int ClientStream::DoConnectCompleted(int result)
	{
		if (result < 0)
			return result;

		// check if message is received completely
		std::unique_ptr<Message> msg;
		msg.reset(m_msg_factory->getMessage(m_inbuf.get()));
		if (!msg) {
			m_next_state = HERMES_CONNECT_PENDING;
			return result;
		}

		LOG(LOG_DEBUG) << "Message received: " << msg->base.opcode << " size=" << msg->base.size;

		// message received - what is it
		switch (msg->base.opcode) {
		case HERMES_ERR_GENERAL:
		case HERMES_ERR_PROTOCOL_VERSION:
			m_oncompletion_handler(msg->base.opcode);
			break;
		case HERMES_MSG_CONNECTED:
			m_oncompletion_handler(0);
			break;
		}
		// exit DoLoop - wait for next instruction
		return -1;
	}

	int ClientStream::DoLoginPending(int result)
	{
		if (result < 0)
			return result;

		m_next_state = HERMES_STATE::HERMES_LOGIN_COMPLETED;

		return m_sock->Read(m_inbuf.get(), [&](int result) -> int {		
				CallDoLoop(result);
				return -1;			
			});
	}

	int ClientStream::DoLoginCompleted(int result)
	{
		if (result < 0)
			return result;

		// check if message is received completely
		std::unique_ptr<Message> msg;
		msg.reset(m_msg_factory->getMessage(m_inbuf.get()));
		if (!msg) {
			m_next_state = HERMES_LOGIN_PENDING;
			return result;
		}

		LOG(LOG_DEBUG) << "Message received: " << msg->base.opcode << " size=" << msg->base.size;

		// message received - what is it
		switch (msg->base.opcode) {
		case HERMES_ERR_GENERAL:
		case HERMES_PROTOCOL_VERSION:
			if (m_err_handler)
				m_err_handler(msg->base.opcode);
			else
				m_login_handler(msg->base.opcode);
			break;
		case HERMES_MSG_LOGIN_FAILED:
		case HERMES_MSG_CREATE_USER_FAILED:
			if (m_login_handler)
				m_login_handler(msg->base.opcode);
			break;
		case HERMES_MSG_LOGIN_SUCCEEDED:
			if (m_login_handler)
				m_login_handler(0);
			break;
		}
		// exit DoLoop - wait for next instruction
		return -1;
	}

	int ClientStream::DoReadMessage(int result)
	{
		if (result < 0)
			return result;

		m_next_state = HERMES_STATE::HERMES_READMESSAGE_COMPLETED;

		return m_sock->Read(m_inbuf.get(), [&](int result) -> int {
			{
				CallDoLoop(result);
				return -1;
			}
			});
	}

	int ClientStream::DoReadMessageCompleted(int result)
	{
		if (result < 0) {
			// this could destroy |this|
			m_message_handler(nullptr, result);
			return -1;
		}

		// check if message is received completely
		std::unique_ptr<Message> msg;
		msg.reset(m_msg_factory->getMessage(m_inbuf.get()));
		if (msg) {
			DLOG(LOG_DEBUG) << "received message with opcode=" << msg->getOpcode();
			m_message_handler(std::move(msg), 0);
			if (m_inbuf->Remaining()) {
				m_next_state = HERMES_STATE::HERMES_READMESSAGE_COMPLETED;
				return (int)m_inbuf->Remaining();
			}
			else {
				m_next_state = HERMES_STATE::HERMES_READMESSAGE;
				return 0;
			}
		}
		else {
			m_next_state = HERMES_STATE::HERMES_READMESSAGE;
			return 0;
		}
	}
}