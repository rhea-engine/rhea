/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Resources/Resource.h>
#include <Rhea/Resources/ResourceList.h>
#include <Psio/Log.h>

namespace rhea {
	
	Resource::~Resource()
	{
		if (m_buf) {
			free(m_buf);
			m_buf = nullptr;
		}
	}

	void Resource::load(orion::ResourceRequest* req, ReadyHandler handler)
	{
		handler(ReqError::OK);
	}

	void Resource::buildInstance(const InstanceRequest&, InstanceHandler handler)
	{
		if (handler)
			handler(ReqError::NOT_IMPLEMENTED, nullptr);
	}

	void Resource::ensureReady(InstanceHandler handler)
	{
		if (handler)
			handler(ReqError::OK, this);
	}
	
	void Resource::prepare(PrepareHandler handler)
	{
		m_prepared = true;
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
