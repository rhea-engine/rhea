#include <Orion/ResourceStream.h>
#include <Psio/Fs/Fs.h>

#include "TextLoader.h"
#include "Resources/TextResource.h"

namespace Rhea
{
	void TextLoader::load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		if (req) {

			Psio::Stream* rs = req->getStream();
			Psio::IOBuffer* buf = new Psio::IOBuffer(64 * 1024);
			rs->onEnd([buf,app,name,handler](size_t buflen) {

				if (buflen > 0) {
					TextResource* res = new TextResource(name, "//meworla/text");
					char* dest = (char*)malloc(buf->Remaining());
					if (dest) {
						memcpy(dest, buf->reader(), buf->Remaining());
						res->setRaw(buf->Remaining(), dest);
						res->setup(app);
					}
					delete buf;
					handler(ReqError::OK, res);
				}
				else {
					delete buf;
					handler(ReqError::DATA_INVALID, nullptr);
				}
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