#ifndef HERMES_CLIENTSTREAM_H
#define HERMES_CLIENTSTREAM_H

#include "Stream.h"

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
			m_next_state(HERMES_NONE),
			m_stream(std::move(stream))
		{}

		// state handling
		int Connect(std::string world_name, Stream::CompletionHandler handler);
		int Loop();

		typedef std::function<void(int)> LoginHandler;
		typedef std::function<void(std::unique_ptr<Message>, int)> MessageHandler;
		typedef std::function<void(int)> CompletionHandler;

		// send requests

		int Login(std::string method, std::string username, std::vector<std::string> secrets, LoginHandler handler);
		int Register(std::string method, std::string username, std::string name, std::string ext_id, std::vector<std::string> secrets, LoginHandler handler);

		int CreateAvatar(std::string name, std::string type_name);

		virtual int sendMessage(Message* message, Stream::CompletionHandler handler);
		void Close();

		// event handlers

		void onMessage(MessageHandler handler);

		void onError(CompletionHandler handler);

	protected:
		std::unique_ptr<Stream> m_stream;
		std::unique_ptr<Message> m_msg;

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

		MessageHandler m_message_handler;
		CompletionHandler m_oncompletion_handler;
		LoginHandler m_login_handler;
		CompletionHandler m_err_handler;
	};
}

#endif // !HERMES_CLIENTSTREAM_H
