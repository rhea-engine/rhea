#include <Orion/ResourceStream.h>
#include <Orion/os/Fs.h>

#include "ImageLoader.h"
#include "ImageResource.h"

namespace Rhea
{
	void ImageLoader::load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		if (req) {
			char* buf;
			size_t buflen;

			Orion::ResourceStream* rs = req->getResource()->Open(Orion::Fs::READ | Orion::Fs::BINARY);
			buflen = rs->ReadBuffer(&buf);
			delete rs;
			
			if (buflen > 0) {
				ImageResource* img = new ImageResource(name, "//meworla/image");
				img->setRaw(buflen, buf);
				img->setup(app);
				handler(ReqError::OK, img);
			}
			else {
				handler(ReqError::DATA_INVALID, nullptr);
			}
		}
		else {
			handler(ReqError::RESOURCE_NOT_FOUND, nullptr);
		}
	}
}