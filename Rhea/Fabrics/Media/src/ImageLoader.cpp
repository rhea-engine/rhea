#include <cstring>

#include <Orion/ResourceStream.h>
#include <Psio/Fs/Fs.h>
#include <Psio/Net/IOBuffer.h>

#include "Rhea/Media/ImageLoader.h"
#include "Rhea/Resources/ImageResource.h"

namespace Rhea
{
	void ImageLoader::load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		if (req) {

			Psio::IOBuffer* buf = new Psio::IOBuffer(64 * 1024);
			Psio::Stream* rs = req->getStream();
			rs->SetMode(Psio::Fs::READ | Psio::Fs::BINARY);
			rs->onEnd([name, app, buf, handler](size_t bytes_read) {
				ImageResource* img = new ImageResource(name);
				char* dest = (char*)malloc(buf->Remaining());
				if (dest) {
					memcpy(dest, buf->reader(), buf->Remaining());
					img->setRaw(buf->Remaining(), dest);
					img->setup(app);
				}
				delete buf;
				handler(ReqError::OK, img);
				});
			rs->onError([buf, handler](int err) {
				delete buf;
				handler(ReqError::DATA_INVALID, nullptr);
				});
			rs->ReadAsync(buf);
		}
		else {
			handler(ReqError::RESOURCE_NOT_FOUND, nullptr);
		}
	}
}