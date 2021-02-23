#include "TextResource.h"
#include "Application.h"
#include "Orion/ResourceStream.h"

namespace Rhea
{
	TextResource::~TextResource()
	{
		if (m_buf)
			free(m_buf);
	}

	void TextResource::load(Orion::ResourceRequest* req, ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			return handler(ReqError::OK);

		if (req) {
			Orion::ResourceStream* rs = req->getResource()->Open(Orion::Fs::READ);
			char* buf;
			size_t buflen;
			buflen = rs->ReadBuffer(&buf);
			if (!buflen) {
				int err = rs->error();				
				m_buflen = 0;
				delete rs;
				m_status = Status::NO_ACCESS;
				handler(ReqError::NO_ACCESS);
			}
			else {
				m_buf = buf;
				m_buflen = buflen;
				delete rs;
				m_status = Status::LOADED;
				handler(ReqError::OK);
			}
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
	
	Unit* TextResource::createType(std::string name, std::string type)
	{
		Unit* ret = new TextResource(name, type);
		return ret;
	}
}
