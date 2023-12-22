#ifndef HERMES_SERVERSTREAM_H
#define HERMES_SERVERSTREAM_H

#include "Stream.h"
#include "Messages/MessageConnect.h"
#include "Messages/MessageLogin.h"
#include "Messages/MessageCreateUser.h"

namespace Hermes
{
	class ServerStream
	{
	public:
		ServerStream(std::unique_ptr<Psio::StreamSocket> sock, MessageFactory* msg_factory) :
			ServerStream(std::make_unique<Stream>(std::move(sock), msg_factory))
		{}
		ServerStream(std::unique_ptr<Stream> stream) :
			m_state(HERMES_NONE),
			m_stream(std::move(stream))
		{}

		typedef std::function<void(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets)> CreateHandler;
		typedef std::function<void(std::string method, std::string username, std::vector<std::string> secrets)> LoginHandler;
		typedef std::function<void(std::string name, std::string type_name)> AvatarCreateHandler;

		// state handling

		int Connect();
		int LoginRetry();
		void Close();

		// sending messages
		// send a message
		virtual int sendMessage(Message* message, Stream::CompletionHandler handler);
		int AuthSuccess(Stream::CompletionHandler handler);
		int AuthFailure(Stream::CompletionHandler handler, std::vector<std::string> methods);

		// event handlers

		virtual void onLogin(LoginHandler handler) { m_login_handler = handler; }
		virtual void onCreateUser(CreateHandler handler) { m_create_handler = handler; }
		virtual void onAvatarCreate(AvatarCreateHandler handler) { m_avatar_create_handler = handler; }
		virtual void onConnectionError(Stream::CompletionHandler error_handler) { m_err_handler = error_handler; }
		virtual void onClosed(Stream::CompletionHandler handler) { m_closed_handler = handler; }
		
		// stream functions
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
		void onConnect(MessageConnect* msg);

		// login messages
		void onLoginMessage(MessageLogin* msg);
		void onCreateMessage(MessageCreateUser* msg);

		LoginHandler m_login_handler;
		CreateHandler m_create_handler;
		Stream::CompletionHandler m_err_handler;
		Stream::CompletionHandler m_closed_handler;
		AvatarCreateHandler m_avatar_create_handler;
	};
}

#endif // !HERMES_SERVERSTREAM_H
