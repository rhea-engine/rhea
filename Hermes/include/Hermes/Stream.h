#ifndef HERMES_STREAM_H
#define HERMES_STREAM_H

#include <Psio/Net/Socket/StreamSocket.h>
#include "Message.h"
#include "MessageFactory.h"
#include "Hermes/Messages/MessageConnect.h"

namespace Hermes
{
	class Stream
	{
	public:
		Stream(std::unique_ptr<Psio::StreamSocket> sock, MessageFactory* msg_factory) :
			m_sock(std::move(sock)),
			m_state(HERMES_NONE),
			m_next_state(HERMES_NONE),
			m_msg_factory(*msg_factory),
			m_connected(false),
			m_inbuf(std::make_unique<Psio::IOBuffer>(64 * 1024)),
			m_outbuf(std::make_unique<Psio::IOBuffer>(64 * 1024))
		{}
		virtual ~Stream() = default;

		MessageFactory* getMessageFactory() { return &m_msg_factory; }

		typedef std::function<void(int)> CompletionHandler;

		// send a message
		virtual int sendMessage(Message* message, CompletionHandler handler);

		// start the message reading loop
		virtual int Start();
		int Restart();
		int Connect(std::string world_name, Stream::CompletionHandler handler);

		// close the stream
		virtual void Close();

		// event handling functions
		void onConnection(CompletionHandler on_connection_handler) { m_on_connection_handler = on_connection_handler; }

		// error handler
		void onConnectionError(CompletionHandler error_handler) { m_err_handler = error_handler; }
		void onClosed(CompletionHandler closed_handler) { m_closed_handler = closed_handler; }

	protected:
		enum HERMES_STATE
		{
			HERMES_NONE,
			HERMES_READMESSAGE,
			HERMES_READMESSAGE_COMPLETED,
			HERMES_CLOSE
		};

		std::unique_ptr<Psio::StreamSocket> m_sock;
		std::string m_id;
		std::unique_ptr<Psio::IOBuffer> m_inbuf;
		std::unique_ptr<Psio::IOBuffer> m_outbuf;

		MessageFactory m_msg_factory;

		CompletionHandler m_err_handler;
		CompletionHandler m_closed_handler;
		CompletionHandler m_on_connection_handler;

		int DoLoop(int result);
		void CallDoLoop(int result);
		int DoReadMessage(int result);
		int DoReadMessageCompleted(int result);
		int DoClose(int result);

		// connection message - server
		void onConnect(MessageConnect* msg);
		// connection message - client
		void onConnectedMessage(Message* msg);
		// protocol error message
		void onErrorMessage(Message* msg);

		HERMES_STATE m_state;
		HERMES_STATE m_next_state;

		bool m_connected;
	};
}

#endif // !HERMES_STREAM_H
