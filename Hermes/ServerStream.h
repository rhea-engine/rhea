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

		virtual int onLogin(MessageHandler handler);
		virtual int onMessage(MessageHandler handler);

	protected:
		// state Handling
		enum HERMES_STATE {
			HERMES_NONE,
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
		int DoLoginPending(int result);
		int DoReadLoginCompleted(int result);
		int DoReadMessage(int result);
		int DoReadMessageCompleted(int result);
	};
}

#endif // !HERMES_SERVERSTREAM_H
