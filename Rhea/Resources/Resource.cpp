#include "Resource.h"
#include "ResourceList.h"
#include <Psio/Log.h>

namespace Rhea {
	
	Resource::~Resource()
	{
	}

	void Resource::load(Orion::ResourceRequest* req, ReadyHandler handler)
	{
		handler(ReqError::OK);
	}
	
	void Resource::setRaw(size_t buflen, char* buf)
	{
		// safety check
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