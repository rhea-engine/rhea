#include "Resource.h"
#include "ResourceList.h"
#include <Psio/Log.h>

namespace Rhea {
	
	Resource::~Resource()
	{
		if (m_buf) {
			free(m_buf);
			m_buf = nullptr;
		}
	}

	void Resource::load(Orion::ResourceRequest* req, ReadyHandler handler)
	{
		handler(ReqError::OK);
	}
	
	void Resource::setRaw(size_t buflen, char* buf)
	{
		// delete old content?
		if (m_buf) {
			free(m_buf);
			m_buf = nullptr;
		}

		// reset requested?
		if (buflen && !buf) {
			m_buflen = 0;
			m_buf = nullptr;
		}
		else {
			m_buflen = buflen;
			m_buf = buf;
		}
		m_status = Status::LOADED;
	}
}