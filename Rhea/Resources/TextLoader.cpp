/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/ResourceStream.h>
#include <Psio/Fs/Fs.h>

#include "Rhea/Resources/TextLoader.h"
#include "Rhea/Resources/TextResource.h"

#include <cstring>

namespace rhea
{
	void TextLoader::load(std::string name, Application* app, orion::ResourceRequest* req, ResourceContext context, Resource::LoadedHandler handler)
	{
		if (req) {

			std::string res_id = "//" + req->getRealmName() + req->getPath();

			psio::Stream* rs = req->getStream();
			psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
			rs->onEnd([buf,app,name,handler,res_id](size_t buflen) {

				if (buflen > 0) {
					TextResource* res = new TextResource(name, "//meworla/text");
					char* dest = (char*)malloc(buf->Remaining());
					if (dest) {
						memcpy(dest, buf->reader(), buf->Remaining());
						res->setRaw(buf->Remaining(), dest);
						res->setup(app);
					}
					else {
						delete buf;
						delete res;
						if (handler)
							handler(rhea::Error(Error::Code::UNAVAILABLE, "Could not allocate memory for text resource", res_id), nullptr);
						return;
					}
					delete buf;
					if (handler)
						handler(rhea::Error(), res);
				}
				else {
					delete buf;
					if (handler)
						handler(rhea::Error(Error::Code::INVALID_DATA, "Invalid data", res_id), nullptr);
				}
				});
			rs->onError([buf, handler, res_id](int err) {
				delete buf;
				Error error(Error::Code::RESOURCE_ERROR, "Could not read text resource", res_id);
				error.addContext("stream_error", std::to_string(err));
				if (handler)
					handler(error, nullptr);
				});
			rs->ReadAsync(buf);
		}
		else {
			if (handler)
				handler(rhea::Error(Error::Code::NOT_FOUND, "Resource not found", name), nullptr);
		}
	}
}