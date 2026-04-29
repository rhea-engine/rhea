/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/Orion.h>
#include <Orion/ResourceRequest.h>
#include <Orion/FileStream.h>
#include <Orion/Mime.h>
#include <Psio/Log.h>
#include <Orion/UrlResponseStream.h>

#include <iostream>

namespace orion
{	
	ResourceRequest::ResourceRequest(string rid, Orion *orion) : 
		ResourceRequest(rid, orion, "")
	{
	}

	ResourceRequest::ResourceRequest(string rid, Orion* orion, std::string type) :
		m_orion(orion),
		m_mime_type(type),
		m_realm_name(""),
		m_path(""),
		m_request(nullptr)
	{		
		parseRid(rid);
	}

	ResourceRequest::~ResourceRequest()
	{		
		if (m_request)
			delete m_request;
	}

	string ResourceRequest::getParentPath()
	{
		return m_container_path;
	}

	string ResourceRequest::getBasename()
	{
		return m_basename;
	}

	bool ResourceRequest::isReading()
	{
		if (m_state == ResourceRequest::State::UNSET ||
			m_state == ResourceRequest::State::RESOLVE_FAILED) {
			return false;
		}

		//  resolve was completed - do we have a stream?
		if (m_state == ResourceRequest::State::RESOURCE_RESOLVE_COMPLETED) {
			if (m_stream)
				return true;
			else
				return false;
		}
		return true;
	}

	bool ResourceRequest::isWriting()
	{
		return false;
	}

	std::string ResourceRequest::getErrorMessage(Error err)
	{
		switch (err) {
		case Error::OK:
			return "OK";
		case Error::IO_PENDING:
			return "Data is pending";
		case Error::REALM_NOT_FOUND:
			return "Realm not found";
		case Error::RESOURCE_NOT_FOUND:
			return "Resource not found";
		case Error::NOT_IMPLEMENTED:
			return "Not implemented";
		case Error::NO_ACCESS:
			return "Permission denied";
		case Error::NO_AUTH:
			return "Authentication failed";
		default:
			return "General error";
		}
	}
	
	ResourceRequest::Error ResourceRequest::parseRid(const string rid)
	{
		size_t pathpos;
		size_t realmpos = 2;

		// make sure the rid is in the correct form
		if (rid.compare(0, 2, "//") && rid.compare(0, 2, "\\\\"))
		{
			// no - try to use full string as realm
			realmpos = 0;
		}

		// split the realm part
		pathpos = rid.find_first_of("\\/", realmpos);
		if (pathpos != std::string::npos)
		{
			m_realm_name = rid.substr(realmpos, pathpos-realmpos);
			m_path = rid.substr(pathpos);
			// find basename
			size_t basepos = m_path.find_last_of("\\/");
			if (basepos != std::string::npos) {
				m_basename = m_path.substr(basepos + 1);
				m_container_path = m_path.substr(0, basepos);
			}
		}
		else {
			// no path, only realm
			m_realm_name = rid.substr(realmpos);
			m_path = "";
		}

		// if realm is empty use 'local'
		if (m_realm_name.empty())
			m_realm_name = "local";

		return Error::OK;
	}

	std::string ResourceRequest::getMimeType()
	{
		if (m_mime_type.length())
			return m_mime_type;

		// try by extensions
		std::string mime_res;
		if (MIME::GetFromExtension(m_path, mime_res)) {
			m_mime_type = mime_res;
		}

		return m_mime_type;
	}
	
	int ResourceRequest::ResolveLoop(int result)
	{
		do
		{
			m_state = m_next_state;
			m_next_state = State::UNSET;

			switch (m_state)
			{
			case ResourceRequest::State::UNSET:
				break;
			case ResourceRequest::State::CACHE_RESOLVE:
				result = doCacheResolve(result);
				break;
			case ResourceRequest::State::REALM_RESOLVE:
				result = doResolveRealm(result);
				break;
			case ResourceRequest::State::REALM_RESOLVE_COMPLETED:
				result = doResolveRealmCompleted(result);
				break;
			case ResourceRequest::State::RESOURCE_RESOLVE:
				result = doResolveResource(result);
				break;
			case ResourceRequest::State::RESOURCE_RESOLVE_COMPLETED:
				result = doResolveResourceCompleted(result);
				break;
			case ResourceRequest::State::RESOLVE_FAILED:
				result = doResolveResourceFailed(result);
			default:
				break;
			}

		} while ((result != Error::IO_PENDING) && (m_state != State::UNSET));

		return result;
	}

	ResourceRequest::Error ResourceRequest::resolveRealm(Orion* orion)
	{
		m_orion = orion;
		return Error::OK;
	}

	int ResourceRequest::doCacheResolve(int result)
	{
		m_stream = m_orion->getResourceFromCache(m_realm_name, m_path);
		if (m_stream) {
			m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
		}
		else {
			m_next_state = State::REALM_RESOLVE;;
		}
		return Error::OK;
	}

	int ResourceRequest::doResolveRealm(int result)
	{
		m_next_state = State::REALM_RESOLVE_COMPLETED;

		// if realm already resolved do nothing
		if (m_realm_resolved) {
			return Error::OK;
		}

		// local realm is always resolved
		if (!m_realm_name.compare("local")) {
			m_local = true;
			m_realm_resolved = true;
			return Error::OK;
		}

		// try to get cached value
		Realm* r = m_orion->getRealms()->getRealmCached(m_realm_name);
		if (r) {
			m_realm_resolved = true;
			m_realm = r;
			return Error::OK;
		}

		// get data from realms manager via lookup
		int ret = m_orion->getRealms()->getRealm(m_realm_name, [&](Realm* realm, int error) {
			// got a result
			if (!error) {
				m_realm_resolved = true;
				m_realm = realm;

			}
			onIOComplete(error);
			});
		if (!ret)
			return Error::OK;
		else
			return Error::IO_PENDING;
	}

	int ResourceRequest::doResolveRealmCompleted(int result)
	{
		//  get the realm if not resolved already 
		if (!m_realm_resolved) {
			Realm* r = m_orion->getRealms()->getRealmCached(m_realm_name);
			if (!r) {
				// if we cant find the realm in the cache the lookup failed
				// fail permanently
				m_next_state = State::RESOLVE_FAILED;
				return Error::REALM_NOT_FOUND;
			}
			m_realm = r;
		}

		// TODO call callbacks if any
		m_next_state = State::RESOURCE_RESOLVE;
		return Error::OK;
	}

	int ResourceRequest::resolve(RequestHandler handler)
	{
		m_next_state = State::CACHE_RESOLVE;
		m_handler = handler;
		int result = ResolveLoop(Error::OK);
		if (result != Error::IO_PENDING && m_handler) {
			m_handler(this, result);
		}
		return result;
	}

	void ResourceRequest::onIOComplete(int result)
	{
		result = ResolveLoop(result);
		if (result != Error::IO_PENDING && m_handler) {
			m_handler(this, result);
		}
	}

	int ResourceRequest::doResolveResource(int result)
	{
		// safety check
		if (!m_realm_resolved)
			return Error::IO_PENDING;

		if (m_local) {
			// if we could not find local resource with resolvers
			// report it as missing
			m_next_state = State::UNSET;
			return Error::RESOURCE_NOT_FOUND;
		}

		// Resource from net
		if (m_realm) {
			string path;

			path = m_realm->getResourceUrl();
			if (path == "") {
				return Error::DATA_INVALID;
			}
			if (path.back() == '/' && m_path[0] == '/')
				path += m_path.substr(1);
			else
				path += m_path;
			LOG(LOG_INFO) << "Resource requested: " << path;

			// Request the resource
			m_request = new psio::UrlRequest(m_orion->getIO());

			m_request->OnResponse([&](psio::UrlResponse* res) {
				std::unique_ptr<psio::UrlResponse> resp;
				resp.reset(res);
				m_stream = std::make_unique<UrlResponseStream>(std::move(resp));
				// caller doesn't know about response codes
				// if not 2xx call with error
				int code = res->GetResponseCode();
				int err = 0;
				if (code == 404)
					err = Error::RESOURCE_NOT_FOUND;
				onIOComplete(err);
				});
			m_request->OnError([&](int err) {
				onIOComplete(err);
				});
			// GET the resource
			m_request->Get(path);
			return Error::IO_PENDING;
		}

		// not implemented		
		m_next_state = State::UNSET;
		return Error::NOT_IMPLEMENTED;
	}

	int ResourceRequest::doResolveResourceCompleted(int result)
	{
		m_next_state = State::UNSET;		
		return Error::OK;
	}

	int ResourceRequest::doResolveResourceFailed(int result)
	{		
		m_next_state = State::UNSET;

		// the request may be deleted -> exit loop
		return Error::RESOURCE_NOT_FOUND;
	}
}