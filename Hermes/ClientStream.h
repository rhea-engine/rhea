#ifndef HERMES_CLIENTSTREAM_H
#define HERMES_CLIENTSTREAM_H

#include "Stream.h"
#include <Hermes/Messages/MessageLoginFailed.h>

namespace Hermes
{
	class ClientStream
	{
	public:
		ClientStream(std::unique_ptr<Psio::StreamSocket> sock, MessageFactory* msg_factory) :
			ClientStream(std::make_unique<Stream>(std::move(sock), msg_factory))
		{}
		ClientStream(std::unique_ptr<Stream> stream) :
			m_state(HERMES_NONE),
			m_stream(std::move(stream))
		{}

		// state handling
		int Connect(std::string world_name, Stream::CompletionHandler handler);
		int Loop();

		typedef std::function<void(int)> LoginHandler;
		typedef std::function<void(int)> CompletionHandler;

		// send requests

		int Login(std::string method, std::string username, std::vector<std::string> secrets, LoginHandler handler);
		int Register(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets, LoginHandler handler);

		int CreateAvatar(std::string name, std::string type_name);

		virtual int sendMessage(Message* message, Stream::CompletionHandler handler);
		void Close();

		// event handlers

		void onError(CompletionHandler handler);

		MessageFactory* getMessageFactory() { return m_stream->getMessageFactory(); }

	protected:
		std::unique_ptr<Stream> m_stream;
		std::unique_ptr<Message> m_msg;

		// state Handling
		enum HERMES_STATE {
			HERMES_NONE,
			HERMES_CONNECT_PENDING,
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

		// error message
		void onErrorMessage(Message* msg);

		CompletionHandler m_oncompletion_handler;
		LoginHandler m_login_handler;
		CompletionHandler m_err_handler;
	};
}

#endif // !HERMES_CLIENTSTREAM_H
