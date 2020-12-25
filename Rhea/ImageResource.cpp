#include "ImageResource.h"

#include "Application.h"
#include "Orion/ResourceStream.h"

namespace Rhea
{
	ImageResource::~ImageResource()
	{		
	}

	void ImageResource::load(Orion::ResourceRequest* req, ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			return handler(ReqError::OK);

		if (req) {
			Orion::ResourceStream* rs = req->getResource()->Open(Orion::Fs::READ | Orion::Fs::BINARY);
			m_buflen = rs->ReadBuffer(&m_buf);
			delete rs;
			if (m_buflen > 0) {
				m_status = Status::LOADED;
				handler(ReqError::OK);
			}
			else {
				m_status = Status::DATA_INVALID;
				handler(ReqError::DATA_INVALID);
			}
		}
		else {
			m_status = Status::NOT_FOUND;
			handler(ReqError::RESOURCE_NOT_FOUND);
		}
	}

	Unit* ImageResource::createType(std::string name, std::string type)
	{
		Unit* ret = new ImageResource(name, type);
		return ret;
	}
}
