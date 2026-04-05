#ifndef HERMES_CLIENTSTREAM_H
#define HERMES_CLIENTSTREAM_H

#include "Stream.h"
#include <Hermes/Messages/MessageLoginFailed.h>

namespace hermes
{
	class ClientStream
	{
	public:
		ClientStream(Stream* stream) :
			m_state(HERMES_NONE),
			m_stream(stream)
		{}

		// state handling
		void Initialize();
		int Loop();

		void Start() { m_state = HERMES_LOGIN_PENDING; }
		void Stop() { m_state = HERMES_NONE; }

		typedef std::function<void(int)> LoginHandler;

		// send requests

		int Login(std::string method, std::string username, std::vector<std::string> secrets, LoginHandler handler);
		int Register(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets, LoginHandler handler);

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
		void onLoginMessage(Message* msg);
		void onLoginFailureMessage(MessageLoginFailed* msg);

		LoginHandler m_login_handler;
	};
}

#endif // !HERMES_CLIENTSTREAM_H
