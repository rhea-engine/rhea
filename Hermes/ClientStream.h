#ifndef HERMES_CLIENTSTREAM_H
#define HERMES_CLIENTSTREAM_H

#include "Stream.h"
#include <Hermes/Messages/MessageLoginFailed.h>

namespace Hermes
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
		typedef std::function<void(int)> CompletionHandler;

		// send requests

		int Login(std::string method, std::string username, std::vector<std::string> secrets, LoginHandler handler);
		int Register(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets, LoginHandler handler);

		virtual int sendMessage(Message* message, Stream::CompletionHandler handler);
		void Close();

		// event handlers

		void onError(CompletionHandler handler);

		MessageFactory* getMessageFactory() { return m_stream->getMessageFactory(); }

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

		// connection message
		void onConnectedMessage(Message* msg);

		void onLoginMessage(Message* msg);

		// login messages
		void onLoginFailureMessage(MessageLoginFailed* msg);

		CompletionHandler m_oncompletion_handler;
		LoginHandler m_login_handler;
		CompletionHandler m_err_handler;
	};
}

#endif // !HERMES_CLIENTSTREAM_H
