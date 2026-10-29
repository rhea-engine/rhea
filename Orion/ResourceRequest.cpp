/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/Orion.h>
#include <Orion/ResourceRequest.h>
#include <Orion/FileStream.h>
#include <Orion/FileCollection.h>
#include <Orion/Mime.h>
#include <Psio/Log.h>
#include <Orion/UrlResponseStream.h>
#include <Orion/Realms.h>

#include <iostream>
#include <algorithm>

namespace orion
{	
	ResourceRequest::ResourceRequest(std::string rid, Orion *orion) : 
		ResourceRequest(rid, orion, "")
	{
	}

	ResourceRequest::ResourceRequest(std::string rid, Orion* orion, std::string type, std::string base_address) :
		m_orion(orion),
		m_mime_type(type),
		m_realm_name(""),
		m_path(""),
		m_request(nullptr),
		m_base_address(base_address)
	{		
		m_error = parseRid(rid);
	}

	ResourceRequest::~ResourceRequest()
	{
		m_stream.reset();
		if (m_request)
			delete m_request;
	}

	bool orion::ResourceRequest::isCompleted()
	{
		if (m_stream)
			return m_stream->state() == psio::Stream::StreamState::STREAM_CLOSED || m_stream->state() == psio::Stream::StreamState::STREAM_ERROR;
		else return false;
	}

	std::string ResourceRequest::getParentPath()
	{
		return m_container_path;
	}

	std::string ResourceRequest::getBasename()
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
	
	Error ResourceRequest::parseRid(std::string rid)
	{
		std::replace(m_base_address.begin(), m_base_address.end(), '\\', '/');
		std::replace(rid.begin(), rid.end(), '\\', '/');

		// fallback for errors
		m_error = Error::DATA_INVALID;

		auto getRealm = [](std::string rid) -> std::string {
			if (rid.compare(0, 2, "//") != 0)
				return std::string();
			size_t realmpos = 2;
			size_t pathpos = rid.find_first_of("/", realmpos);
			if (pathpos != std::string::npos)
				return rid.substr(realmpos, pathpos - realmpos);
			else
				return rid.substr(realmpos);
			};

		// check if we have a realm part - if not we resolve it as local path
		if (rid.compare(0, 2, "//") != 0) {
			if (m_base_address.empty())
				return Error::DATA_INVALID;

			if (rid.empty())
				return Error::DATA_INVALID;

			if (rid.front() == '/') {
				// Keep the base realm and replace its entire path.
				std::string base_realm = getRealm(m_base_address);
				if (base_realm.empty())
					return Error::DATA_INVALID;
				rid = "//" + base_realm + rid;
			}
			else {
				// Append rid to the base directory.
				std::string relative_path = rid;
				rid = m_base_address;
				if (rid.back() != '/')
					rid += '/';
				rid += relative_path;
			}
		}

		// split the realm part
		m_realm_name = getRealm(rid);
		m_path = rid.substr(2 + m_realm_name.length());

		// normalize '.' and '..' in the path
		std::vector<std::string> segments;
		const bool trailing_slash = !m_path.empty() && m_path.back() == '/';

		size_t begin = 0;
		while (begin < m_path.size()) {
			size_t end = m_path.find('/', begin);
			if (end == std::string::npos)
				end = m_path.size();

			const std::string segment = m_path.substr(begin, end - begin);

			if (segment == "..") {
				size_t root_depth = 0;

#if defined(_WIN32)
				if (m_realm_name == "file")
					root_depth = !segments.empty() && segments[0] == "unc" ? 3 : 1;
#endif

				if (segments.size() <= root_depth)
					return Error::DATA_INVALID;

				segments.pop_back();
			}
			else if (!segment.empty() && segment != ".") {
				segments.push_back(segment);
			}

			begin = end + 1;
		}

		m_path = "/";
		for (const auto& segment : segments) {
			if (m_path.back() != '/')
				m_path += '/';

			m_path += segment;
		}

		if (trailing_slash && m_path.back() != '/')
			m_path += '/';

		// find basename
		size_t basepos = m_path.find_last_of("/");
		if (basepos != std::string::npos) {
			m_basename = m_path.substr(basepos + 1);
			m_container_path = m_path.substr(0, basepos);
		}

		// if realm is empty return error
		if (m_realm_name.empty())
			return Error::DATA_INVALID;

		m_error = Error::OK;
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
	
	Error ResourceRequest::ResolveLoop(Error result)
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
				break;
			default:
				break;
			}

		} while ((result != Error::IO_PENDING) && (m_state != State::UNSET));

		return result;
	}

	Error ResourceRequest::resolveRealm(Orion* orion)
	{
		m_orion = orion;
		return Error::OK;
	}

	Error ResourceRequest::doCacheResolve(Error result)
	{
		if (m_realm_name == "file") {
			m_next_state = State::REALM_RESOLVE;
			return Error::OK;
		}

		m_stream = m_orion->getResourceFromCache(m_realm_name, m_path);
		if (m_stream) {
			m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
		}
		else {
			m_next_state = State::REALM_RESOLVE;;
		}
		return Error::OK;
	}

	Error ResourceRequest::doResolveRealm(Error result)
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
		Error err = m_orion->getRealms()->getRealmCached(m_realm_name, m_realm);
		if (err == Error::OK) {
			m_realm_resolved = true;
			return Error::OK;
		}

		// if file realm is not defined dont allow it to be resolved
		if (m_realm_name == "file") {
			m_next_state = State::RESOLVE_FAILED;
			return Error::REALM_NOT_FOUND;
		}

		// get data from realms manager via lookup
		Error ret = m_orion->getRealms()->getRealm(m_realm_name, [&](Realm realm, orion::Error error) {
			// got a result
			if (error == orion::Error::OK) {
				m_realm_resolved = true;
				m_realm = realm;

			}
			onIOComplete(error);
			});
		if (ret == Error::OK)
			return Error::OK;
		else
			return Error::IO_PENDING;
	}

	Error ResourceRequest::doResolveRealmCompleted(Error result)
	{
		if (result != Error::OK) {
			m_next_state = State::RESOLVE_FAILED;
			return result;
		}

		//  get the realm if not resolved already 
		if (!m_realm_resolved) {
			Error err = m_orion->getRealms()->getRealmCached(m_realm_name, m_realm);
			if (err != Error::OK) {
				// if we cant find the realm in the cache the lookup failed
				// fail permanently
				m_next_state = State::RESOLVE_FAILED;
				return Error::REALM_NOT_FOUND;
			}
		}

		// TODO call callbacks if any
		m_next_state = State::RESOURCE_RESOLVE;
		return Error::OK;
	}

	Error ResourceRequest::resolve(RequestHandler handler)
	{
		m_next_state = State::CACHE_RESOLVE;
		m_handler = handler;
		if (m_error != Error::OK) {
			m_next_state = State::RESOLVE_FAILED;
			if (m_handler) {
				m_handler(this, m_error);
			}
			return m_error;
		}
		Error result = ResolveLoop(Error::OK);
		if (result != Error::IO_PENDING && m_handler) {
			m_handler(this, result);
		}
		return result;
	}

	void ResourceRequest::onIOComplete(Error result)
	{
		result = ResolveLoop(result);
		if (result != Error::IO_PENDING && m_handler) {
			m_handler(this, result);
		}
	}

	Error ResourceRequest::doResolveResource(Error result)
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
		if (m_realm.getType() != Realm::RealmType::Unset) {
			std::string path;

			if (m_realm.isLocalPath() || m_realm.getType() == Realm::RealmType::FileSystem) {
#if defined(_WIN32)
				if (m_realm.getType() == Realm::RealmType::FileSystem && m_path == "/") {
					m_stream = std::make_unique<FileCollection>(psio::Fs::getLogicalDrives());
					m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
					onIOComplete(Error::OK);
					return Error::IO_PENDING;
				}
#endif
				std::string native_path;
				const Error error = m_realm.resolveFilePath(m_path, native_path);
				if (error != Error::OK) {
					m_next_state = State::UNSET;
					return error;
				}

				m_stream = FileResourceStream::getStream(native_path);
				if (!m_stream) {
					m_next_state = State::UNSET;
					return Error::RESOURCE_NOT_FOUND;
				}
				m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
				onIOComplete(Error::OK);
				return Error::IO_PENDING;
			}

			path = m_realm.getResourceUrl();
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

			m_request->OnResponse([&](psio::UrlResponse* resp) {
				m_stream = std::make_unique<UrlResponseStream>(resp);
				// caller doesn't know about response codes
				// if not 2xx call with error
				int code = resp->GetResponseCode();
				Error err = Error::OK;
				if (code == 404)
					err = Error::RESOURCE_NOT_FOUND;
				else if (code == 403)
					err = Error::NO_ACCESS;
				else if (code == 401)
					err = Error::NO_AUTH;
				else if (code < 200 || code >= 300)
					err = Error::DATA_INVALID;
				onIOComplete(err);
				});
			m_request->OnError([&](int err) {
				onIOComplete(translateTransportError(err));
				});
			// GET the resource
			m_request->Get(path);
			return Error::IO_PENDING;
		}

		// not implemented		
		m_next_state = State::UNSET;
		return Error::NOT_IMPLEMENTED;
	}

	Error ResourceRequest::doResolveResourceCompleted(Error result)
	{
		m_next_state = State::UNSET;		
		return result;
	}

	Error ResourceRequest::doResolveResourceFailed(Error result)
	{		
		m_next_state = State::UNSET;

		// the request may be deleted -> exit loop
		return result;
	}
}
