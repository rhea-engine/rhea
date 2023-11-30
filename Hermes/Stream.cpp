#include "Stream.h"
#include <Psio/Log.h>

namespace Hermes
{
	int Stream::sendMessage(Message* message, CompletionHandler handler)
	{
		message->serialize(m_outbuf.get());
		static uint16_t lastopcode = 0;
		static uint16_t repeat = 0;
		if (lastopcode != message->base.opcode) {
			DLOG(LOG_DEBUG) << "Sending msg " << message->base.opcode << ". Size=" << m_outbuf->Remaining();
			if (repeat)
				DLOG(LOG_DEBUG) << "repeated " << repeat << " times";
			lastopcode = message->base.opcode;
			repeat = 0;
		}
		else
			repeat++;
		return m_sock->Write(m_outbuf.get(), [handler](int err) {
			if (handler) {
				handler(err);
			}
			});
	}

}