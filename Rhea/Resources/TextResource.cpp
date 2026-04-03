#include <Rhea/Resources/TextResource.h>
#include <Rhea/Application.h>
#include "Orion/ResourceStream.h"

#include <cstring>

namespace Rhea
{
	TextResource::~TextResource()
	{
	}

	void TextResource::load(Orion::ResourceRequest* req, ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			return handler(ReqError::OK);

		if (req) {
			Psio::Stream* rs = req->getStream();
			Psio::IOBuffer* buf = new Psio::IOBuffer(64 * 1024);
			rs->onEnd([&](size_t buflen) {
				if (!buflen) {
					m_buflen = 0;
					m_status = Status::NO_ACCESS;
					delete buf;
					handler(ReqError::NO_ACCESS);
				}
				else {
					m_buf = (char*)malloc(buflen);
					if (m_buf) {
						memcpy(m_buf, buf->reader(), buflen);
						m_buflen = buflen;
					}
					delete buf;
					m_status = Status::LOADED;
					handler(ReqError::OK);
				}
				});
			rs->onError([buf, handler](int err) {
				delete buf;
				handler(ReqError::DATA_INVALID);
				});
			rs->ReadAsync(buf);
		}
		else {
			m_status = Status::NOT_FOUND;
			handler(ReqError::RESOURCE_NOT_FOUND);
		}
	}
	
	std::string TextResource::getAsString()
	{
		return std::string(m_buf, m_buflen);
	}
}
