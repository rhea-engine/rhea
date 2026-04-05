#ifndef HERMES_MESGFACTORY_H
#define HERMES_MESGFACTORY_H

#include <unordered_map>
#include <functional>
#include "Message.h"
#include <Psio/Net/IOBuffer.h>


namespace hermes
{
	class MessageFactory
	{
	public:
		typedef std::function<Message*(uint16_t, uint16_t, psio::IOBuffer*)> CreatorFunction;
		typedef std::function<void(Message*)> HandlerCaller;
		typedef std::function<bool(uint16_t)> MessageFilter;
		
		void registerMessageType(uint16_t opcode, std::string name, CreatorFunction creator);
		Message* getMessage(psio::IOBuffer* buf);
		// helper for core messages
		void registerCoreMessages();

		template<typename MsgType>
		void registerMessage(uint16_t opcode, std::string name, CreatorFunction creator, MessageFilter filter, std::function<void(MsgType*)> handler)
		{
			m_info[opcode].creator = creator;
			if (handler) {
			m_info[opcode].handler = [handler](Message* msg) {
				handler(static_cast<MsgType*>(msg));
				};
			}
			if (filter)
				m_info[opcode].filter = filter;
			m_info[handler].name = name;
		}

		// register message handler and filter for already registered message
		template<typename MsgType>
		void registerMessageHandler(uint16_t opcode, MessageFilter filter, std::function<void(MsgType*)> handler)
		{
			m_info[opcode].handler = [handler](Message* msg) {
				handler(static_cast<MsgType*>(msg));
				};
			if (filter)
				m_info[opcode].filter = filter;			
		}

		bool handle(Message* msg);
		bool filter(uint16_t opcode);

		std::string getMessageName(uint16_t opcode);

	protected:
		struct MessageInfo
		{
			CreatorFunction creator;
			HandlerCaller handler;
			MessageFilter filter;
			std::string name;
		};
		std::unordered_map<uint16_t, MessageInfo> m_info;
	};
}

#endif // !HERMES_MESGFACTORY_H
