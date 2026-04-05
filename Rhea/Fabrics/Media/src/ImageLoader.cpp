#include <cstring>

#include <Orion/ResourceStream.h>
#include <Psio/Fs/Fs.h>
#include <Psio/Net/IOBuffer.h>

#include "Rhea/Media/ImageLoader.h"
#include "Rhea/Resources/ImageResource.h"

namespace rhea
{
	void ImageLoader::load(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		if (req) {

			psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
			psio::Stream* rs = req->getStream();
			rs->SetMode(psio::Fs::READ | psio::Fs::BINARY);
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