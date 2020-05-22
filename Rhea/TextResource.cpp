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

	void TextResource::load(ReadyHandler handler)
	{
		if (m_status>=Status::LOADED)
			return handler(ReqError::OK);

		m_app->loadResource(m_path, [&](Orion::ResourceRequest* req, Orion::ResourceRequest::Error err) {
			if (req) {
				Orion::ResourceStream* rs = req->getResource()->Open(Orion::Fs::READ);
				char* buf;
				size_t buflen;
				buflen = rs->ReadBuffer(&buf);
				delete rs;
				if (buflen > 0) {
					m_buf = buf;
					m_buflen = buflen;
				}
				m_status = Status::LOADED;
				handler(ReqError::OK);
			}
			else {
				m_status = Status::NOT_FOUND;
				handler(ReqError::RESOURCE_NOT_FOUND);
			}
			});
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
