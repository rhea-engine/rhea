#ifndef HERMES_MESGFACTORY_H
#define HERMES_MESGFACTORY_H

#include <unordered_map>
#include <functional>
#include "Message.h"
#include <Psio/Net/IOBuffer.h>


namespace Hermes
{
	class MessageFactory
	{
	public:
		typedef std::function<Message*(uint16_t, uint16_t, Psio::IOBuffer*)> CreatorFunction;
		void registerMessageType(uint16_t opcode, CreatorFunction creator);
		Message* getMessage(Psio::IOBuffer* buf);
		// helper for core messages
		void registerCoreMessages();
	protected:
		std::unordered_map<uint16_t, CreatorFunction> m_creators;
	};
}

#endif // !HERMES_MESGFACTORY_H
