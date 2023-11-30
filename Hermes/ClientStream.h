#ifndef HERMES_CLIENTSTREAM_H
#define HERMES_CLIENTSTREAM_H

#include "Stream.h"

namespace Hermes
{
	class ClientStream : public Stream
	{
	public:
		ClientStream(std::unique_ptr<Psio::StreamSocket> sock, MessageFactory* msg_factory) :
			m_state(HERMES_NONE),
			m_next_state(HERMES_NONE),
			Stream(std::move(sock), msg_factory)
		{}

		int Connect(std::string world_name, CompletionHandler handler);

		typedef std::function<void(int)> LoginHandler;
		void Login(std::string method, std::string username, std::vector<std::string> secrets, LoginHandler handler);

		int onMessage(MessageHandler handler);

		void onError(CompletionHandler handler);

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
			HERMES_READMESSAGE_COMPLETED
		};

		HERMES_STATE m_state;
		HERMES_STATE m_next_state;
		int DoLoop(int result);
		void CallDoLoop(int result);
		int DoConnectPending(int result);
		int DoConnectCompleted(int result);
		int DoLoginPending(int result);
		int DoLoginCompleted(int result);
		int DoReadMessage(int result);
		int DoReadMessageCompleted(int result);

		CompletionHandler m_oncompletion_handler;
		LoginHandler m_login_handler;
		CompletionHandler m_err_handler;
	};
}

#endif // !HERMES_CLIENTSTREAM_H
