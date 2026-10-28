/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Resources/TextResource.h>
#include <Rhea/Application.h>
#include "Orion/ResourceStream.h"

#include <cstring>

namespace rhea
{
	TextResource::~TextResource()
	{
	}

	void TextResource::load(orion::ResourceRequest* req, ReadyHandler handler)
	{
		if (m_status >= Status::LOADED) {
			if (handler)
				handler(rhea::Error());
			return;
		}

		if (req) {
			psio::Stream* rs = req->getStream();
			psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
			std::string res_id = "//" + req->getRealmName() + req->getPath();

			rs->onEnd([res_id, this, buf, handler](size_t buflen) {
				if (!buflen) {
					m_buflen = 0;
					m_status = Status::NO_ACCESS;
					delete buf;
					if (handler)
						handler(rhea::Error(Error::Code::ACCESS_DENIED, "No access to resource", res_id));
				}
				else {
					m_buf = (char*)malloc(buflen);
					if (m_buf) {
						memcpy(m_buf, buf->reader(), buflen);
						m_buflen = buflen;
					}
					else {
						delete buf;
						if (handler)
							handler(rhea::Error(Error::Code::UNAVAILABLE, "Could not allocate memory for text resource", res_id));
						return;
					}
					delete buf;
					m_status = Status::LOADED;
					if (handler)
						handler(rhea::Error());

				}
				});
			rs->onError([buf, handler, res_id](int err) {
				delete buf;
				if (handler)
					handler(rhea::Error(Error::Code::INVALID_DATA, "Invalid data", res_id));
				});
			rs->ReadAsync(buf);
		}
		else {
			m_status = Status::NOT_FOUND;
			if (handler)
				handler(rhea::Error(Error::Code::NOT_FOUND, "Resource not found"));
		}
	}
	
	std::string TextResource::getAsString()
	{
		return std::string(m_buf, m_buflen);
	}
}
