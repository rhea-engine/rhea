/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/RealmRequest.h>
#include <Orion/Orion.h>
#include <Psio/Log.h>
#include <Orion/Mime.h>

#include <nlohmann/json.hpp>
#include <stdexcept>

#include "RealmJson.h"

namespace orion
{
	RealmRequest::RealmRequest(Orion* orion,std::deque<std::string>& resolve_paths) :
		m_error(0),
		m_orion(orion),
		m_request(nullptr),
		m_resolve_paths(resolve_paths)
	{

	}
	
	RealmRequest::~RealmRequest()
	{
		if (m_request)
			delete m_request;
	}
	
	Error RealmRequest::lookup(std::string realm_name, RealmHandler handler)
	{
		if (m_resolve_paths.empty()) {
			return Error::REALM_NOT_FOUND;
		}

		m_request = new psio::UrlRequest(m_orion->getIO());		
		// call UrlRequest
		m_request->OnResponse([handler, realm_name](psio::UrlResponse* res) {
			// got a response	

			// check for http error
			int code = res->GetResponseCode();
			if (code != 200) {
				if (handler) {
					if (code == 404) {
						handler(nullptr, Error(Error::REALM_NOT_FOUND));
					} else if (code == 403) {
						handler(nullptr, Error(Error::NO_ACCESS));
					} else if (code == 401) {
						handler(nullptr, Error(Error::NO_AUTH));
					} else {
						handler(nullptr, Error(Error::DATA_INVALID));
					}
				}
				return;
			}
			std::string content_type;
			MIME::GetFromContentType(res->getContentType(), content_type);
			if (content_type != "application/json") {
				if (handler) {
					handler(nullptr, Error::DATA_INVALID);
				}
				return;
			}

			// try reading the body in IOBuffer buf
			psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
			res->onEnd([buf, handler, realm_name](size_t bytes_read) {
				// success -  we got the result in body
				// the result is json -> parse it
				std::string response = std::string(buf->data(), bytes_read);
				delete buf;
				Realm* realm = nullptr;
				try {
					nlohmann::json j = nlohmann::json::parse(response);
					realm = new Realm(j.get<Realm>());					
				}
				catch (const nlohmann::json::exception& e) {
					LOG(LOG_ERROR) << "RealmRequest: JSON parse error while parsing realm JSON: " << e.what();
					handler(nullptr, Error::DATA_INVALID);
					return;
				}
				catch (const std::invalid_argument& e) {
					LOG(LOG_ERROR) << "RealmRequest: Invalid argument while parsing realm JSON: " << e.what();
					handler(nullptr, Error::DATA_INVALID);
					return;
				}
				catch (...) {
					handler(nullptr, Error::DATA_INVALID);
					return;
				}
				// check if realm is valid
				if (!realm || realm->getName() != realm_name) {
					delete realm;
					handler(nullptr, Error::DATA_INVALID);
					return;
				}
				handler(realm, Error::OK);
				});
			res->onError([buf, handler](int err) {
				// failed with an error
				delete buf;
				handler(nullptr, translateTransportError(err));
				});
			res->ReadAsync(buf);
			});
		m_request->OnError([handler](int err) {
			handler(nullptr, translateTransportError(err));
			});		
		std::string resolve_path = m_resolve_paths.front();
		m_resolve_paths.pop_front();

		m_request->setFollowRedirects(false);
		if (resolve_path.back() != '/')
			resolve_path.push_back('/');
		m_request->Get(resolve_path + realm_name + ".json");
		return Error::IO_PENDING;
	}
	
	void RealmRequest::cancel()
	{
		if (m_request) {
			// remove the request
			delete m_request;
			m_request = nullptr;
		}
	}
}