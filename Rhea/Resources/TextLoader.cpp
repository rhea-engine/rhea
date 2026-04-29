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
	void TextLoader::load(std::string name, Application* app, orion::ResourceRequest* req, Resource::LoadedHandler handler)
	{
		if (req) {

			psio::Stream* rs = req->getStream();
			psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
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