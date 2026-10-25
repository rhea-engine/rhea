/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/RealmRequest.h>
#include <Orion/Orion.h>

#include <nlohmann/json.hpp>

#include "RealmJson.h"

namespace orion
{
	RealmRequest::RealmRequest(Orion* orion,std::deque<std::string>& iris) :
		m_error(0),
		m_orion(orion),
		m_request(nullptr),
		m_iris(iris)
	{

	}
	
	RealmRequest::~RealmRequest()
	{
		if (m_request)
			delete m_request;
	}
	
	void RealmRequest::Lookup(std::string realm_name, RealmHandler handler)
	{
		m_request = new psio::UrlRequest(m_orion->getIO());		
		// call UrlRequest
		m_request->OnResponse([handler](psio::UrlResponse* res) {
			// got a response
		
			// check for http error
			int code = res->GetResponseCode();
			if (code < 200 || code >= 300) {
				if (handler) {
					if (code == 404) {
						handler(nullptr, Error(Error::RESOURCE_NOT_FOUND));
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
			// try reading the body in IOBuffer buf
			psio::IOBuffer* buf = new psio::IOBuffer(64 * 1024);
			res->onEnd([buf, handler](size_t bytes_read) {
				// success -  we got the result in body
				// the result is json -> parse it
				std::string response = std::string(buf->data(), bytes_read);
				delete buf;
				try {
					nlohmann::json j = nlohmann::json::parse(response);
					Realm* realm = new Realm(j.get<Realm>());
					handler(realm, Error::OK);
				}
				catch (const nlohmann::json::exception& e) {
					handler(nullptr, Error::DATA_INVALID);
				}
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
		if (m_iris.size() < 1) {
			m_iris.push_back("http://localhost/iris/realms");
		}
		std::string iris_server = m_iris.front();
		m_iris.pop_front();

		m_request->Get(iris_server + "/" + realm_name);
	}
	
	void RealmRequest::Cancel()
	{
		if (m_request) {
			// remove the request
			delete m_request;
			m_request = nullptr;
		}
	}
}