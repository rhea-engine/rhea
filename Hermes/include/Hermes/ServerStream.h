#ifndef HERMES_SERVERSTREAM_H
#define HERMES_SERVERSTREAM_H

#include "Stream.h"
#include "Hermes/Messages/MessageConnect.h"
#include "Hermes/Messages/MessageLogin.h"
#include "Hermes/Messages/MessageCreateUser.h"


namespace hermes
{
	class ServerStream 
	{
	public:
		ServerStream(Stream* stream) :
			m_state(HERMES_NONE),
			m_stream(stream)
		{}

		typedef std::function<void(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets)> CreateHandler;
		typedef std::function<void(std::string method, std::string username, std::vector<std::string> secrets)> LoginHandler;

		// initialization
		void Initialize();

		// state handling

		void Start() { m_state = HERMES_LOGIN_PENDING; };
		void Stop() { m_state = HERMES_NONE; }
		int LoginRetry();

		// sending messages
		int AuthSuccess(Stream::CompletionHandler handler);
		int AuthFailure(Stream::CompletionHandler handler, std::vector<std::string> methods);

		// event handlers

		virtual void onLogin(LoginHandler handler) { m_login_handler = handler; }
		virtual void onCreateUser(CreateHandler handler) { m_create_handler = handler; }

	protected:
		Stream* m_stream;
		std::unique_ptr<Message> m_msg;

		// state Handling
		enum HERMES_STATE {
			HERMES_NONE,
			HERMES_LOGIN_PENDING,
			HERMES_SEND_SETUP,
			HERMES_READMESSAGE,
		};

		HERMES_STATE m_state;

		// login messages
		void onLoginMessage(MessageLogin* msg);
		void onCreateMessage(MessageCreateUser* msg);

		LoginHandler m_login_handler;
		CreateHandler m_create_handler;
	};
}

#endif // !HERMES_SERVERSTREAM_H
