#include "ImageResource.h"

#include "Application.h"
#include "Orion/ResourceStream.h"

namespace Rhea
{
	ImageResource::~ImageResource()
	{
		if (m_img)
			delete m_img;
	}

	void ImageResource::load(ReadyHandler handler)
	{
		if (m_status>=Status::LOADED)
			return handler(ReqError::OK);

		m_app->loadResource(m_path, [&](Orion::ResourceRequest* req, Orion::ResourceRequest::Error err) {
			
			if (req) {
				Orion::ResourceStream* rs = req->getResource()->Open(Orion::Fs::READ | Orion::Fs::BINARY);
				char* buf;
				size_t buflen;
				buflen = rs->ReadBuffer(&buf);
				if (buflen > 0) {
					// load texture
					Poseidon::Texture* tex;
					tex = m_app->get_core()->createTexture((unsigned char*)buf, buflen);
					if (!tex) {
						m_app->log("Loading of texture failed\n");
						m_status = Status::DATA_INVALID;
						handler(ReqError::DATA_INVALID);
					}
					m_status = Status::LOADED;
					m_img = (Poseidon::Image*)tex;
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
			});
	}

	Unit* ImageResource::createType(std::string name, std::string type)
	{
		Unit* ret = new ImageResource(name, type);
		return ret;
	}
}
