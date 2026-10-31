/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <cstring>

#include <Orion/ResourceStream.h>
#include <Psio/Fs/Fs.h>
#include <Psio/Net/IOBuffer.h>

#include "Rhea/Media/ImageLoader.h"
#include "Rhea/Resources/ImageResource.h"

namespace rhea
{
	void ImageLoader::load(std::string name, Application* app, orion::ResourceRequest* req, ResourceContext context, Resource::LoadedHandler handler)
	{
		if (req) {

			psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
			psio::Stream* rs = req->getStream();
			std::string res_id = "//" + req->getRealmName() + req->getPath();
			rs->SetMode(psio::Fs::READ | psio::Fs::BINARY);
			rs->onEnd([name, app, buf, handler, res_id](size_t bytes_read) {
				ImageResource* img = new ImageResource(name);
				char* dest = (char*)malloc(buf->Remaining());
				if (dest) {
					memcpy(dest, buf->reader(), buf->Remaining());
					img->setRaw(buf->Remaining(), dest);
					img->setup(app);
				}
				else {
					delete img;
					delete buf;
					if (handler)
						handler(rhea::Error(Error::Code::UNAVAILABLE, "Could not allocate memory for image resource", res_id), nullptr);
					return;
				}
				delete buf;
				if (handler)
					handler(Error(), img);
				});
			rs->onError([buf, handler, res_id](int err) {
				delete buf;
				Error error(Error::Code::RESOURCE_ERROR, "Could not read image resource", res_id);
				error.addContext("stream_error", std::to_string(err));
				if (handler)
					handler(error, nullptr);
				});
			rs->ReadAsync(buf);
		}
		else {
			Error error(Error::Code::NOT_FOUND, "Resource not found.");
			if (handler)
				handler(error, nullptr);
		}
	}
}