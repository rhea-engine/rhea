#ifndef HERMES_SERVERSTREAM_H
#define HERMES_SERVERSTREAM_H

#include "Stream.h"

namespace Hermes
{
	class ServerStream : public Stream
	{
	public:
		ServerStream(std::unique_ptr<Psio::StreamSocket> sock, MessageFactory* msg_factory) :
			m_state(HERMES_NONE),
			m_next_state(HERMES_NONE),
			Stream(std::move(sock), msg_factory)
		{}
		typedef std::function<void(std::unique_ptr<Message>, int)> MessageHandler;
		typedef std::function<void(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets)> CreateHandler;
		typedef std::function<void(std::string method, std::string username, std::vector<std::string> secrets)> LoginHandler;
		typedef std::function<void(std::string name, std::string type_name)> AvatarCreateHandler;

		virtual int onLogin(LoginHandler handler, CompletionHandler err_handler);
		virtual void onCreateUser(CreateHandler handler) { m_create_handler = handler; }
		virtual void onAvatarCreate(AvatarCreateHandler handler) { m_avatar_create_handler = handler; }
		virtual int onMessage(MessageHandler handler);
		int LoginRetry();

		void AuthSuccess(CompletionHandler handler);
		void AuthFailure(CompletionHandler handler, std::vector<std::string> methods);

	protected:
		// state Handling
		enum HERMES_STATE {
			HERMES_NONE,
			HERMES_CONNECT_PENDING,
			HERMES_CONNECT_COMPLETED,
			HERMES_LOGIN_PENDING,
			HERMES_LOGIN_COMPLETED,
			HERMES_SEND_SETUP,
			HERMES_SEND_SETUP_COMPLETED,
			HERMES_READMESSAGE,
			HERMES_READMESSAGE_COMPLETED,
		};

		HERMES_STATE m_state;
		HERMES_STATE m_next_state;
		int DoLoop(int result);
		void CallDoLoop(int result);
		int DoConnectPending(int result);
		int DoConnectCompleted(int result);
		int DoLoginPending(int result);
		int DoReadLoginCompleted(int result);
		int DoReadMessage(int result);
		int DoReadMessageCompleted(int result);

		LoginHandler m_login_handler;
		CreateHandler m_create_handler;
		CompletionHandler m_err_handler;
		AvatarCreateHandler m_avatar_create_handler;
	};
}

#endif // !HERMES_SERVERSTREAM_H
