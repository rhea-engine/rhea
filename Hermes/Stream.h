#ifndef HERMES_STREAM_H
#define HERMES_STREAM_H

#include <Psio/Net/Socket/StreamSocket.h>
#include "Message.h"
#include "MessageFactory.h"

namespace Hermes
{
	class Stream
	{
	public:
		Stream(std::unique_ptr<Psio::StreamSocket> sock, MessageFactory* msg_factory) :
			m_sock(std::move(sock)),
			m_msg_factory(msg_factory)
		{}
		virtual ~Stream() = default;

		typedef std::function<void(std::unique_ptr<Message>, int)> MessageHandler;
		typedef std::function<void(int)> CompletionHandler;

		virtual int sendMessage(Message* message, CompletionHandler handler);

		virtual int onMessage(MessageHandler handler) = 0;

	protected:
		std::unique_ptr<Psio::StreamSocket> m_sock;
		std::unique_ptr<Psio::IOBuffer> m_inbuf;
		std::unique_ptr<Psio::IOBuffer> m_outbuf;

		MessageHandler m_message_handler;
		MessageFactory* m_msg_factory;
	};
}

#endif // !HERMES_STREAM_H
