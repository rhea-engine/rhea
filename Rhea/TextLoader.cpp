#include <Orion/ResourceStream.h>
#include <Orion/os/Fs.h>

#include "TextLoader.h"
#include "TextResource.h"

namespace Rhea
{
	void TextLoader::load(std::string name, Application* app, Orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		if (req) {
			char* buf;
			size_t buflen;

			Orion::ResourceStream* rs = req->getResource()->Open(Orion::Fs::READ);
			buflen = rs->ReadBuffer(&buf);
			delete rs;

			if (buflen > 0) {
				TextResource* res = new TextResource(name, "//meworla/text");
				res->setup(app);
				res->setRaw(buflen, buf);
				handler(ReqError::OK, res);
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