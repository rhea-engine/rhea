/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/Orion.h>
#include <Orion/ResourceRequest.h>
#include <Orion/FileStream.h>
#include <Orion/FileAddress.h>
#include <Orion/FileCollection.h>
#include <Orion/Mime.h>
#include <Psio/Log.h>
#include <Orion/UrlResponseStream.h>
#include <Orion/Realms.h>
#include <Orion/JsonCollection.h>
#include <Orion/BufferStream.h>

#include <iostream>
#include <algorithm>

namespace orion
{
	ResourceRequest::ResourceRequest(const ResourceAddress& address, Orion* orion, ResultKind kind, std::string mime_type) :
		m_address(address),
		m_orion(orion),
		m_request(nullptr),
		m_mime_type(mime_type),
		m_result_kind(kind)
	{
		setAddress(address);
	}

	ResourceRequest::ResourceRequest(std::string rid, Orion* orion, ResultKind kind, std::string type) :
		m_orion(orion),
		m_mime_type(type),
		m_result_kind(kind),
		m_realm_name(""),
		m_path(""),
		m_request(nullptr),
		m_address({})
	{
		AddressResult result = ResourceAddress::parse(rid);
		if (!result.success) {
			m_error = Error::DATA_INVALID;
			return;
		}
		setAddress(result.address);
	}

	ResourceRequest::~ResourceRequest()
	{
		m_stream.reset();
		if (m_request)
			delete m_request;

		if (m_orion && m_realm_request_generation > 0) {
			m_orion->getRealms()->cancelResolve(m_realm_name, m_realm_request_generation);
		}
	}

	void orion::ResourceRequest::setAddress(ResourceAddress address)
	{
		m_address = address;
		m_realm_name = address.authority();
		m_path = address.path();
		// find basename
		size_t basepos = m_path.find_last_of("/");
		if (basepos != std::string::npos) {
			m_basename = m_path.substr(basepos + 1);
			m_container_path = m_path.substr(0, basepos);
		}
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

	void ResourceRequest::setMimeTypeFromExtension()
	{
		std::string mime_res;
		if (MIME::GetFromExtension(m_path, mime_res)) {
			m_mime_type = mime_res;
		}
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
		m_stream = m_orion->getResourceFromCache(m_realm_name, m_path);
		if (m_stream) {
			m_effective_address = m_address;
			m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
		}
		else {
			m_next_state = State::REALM_RESOLVE;
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
		Error ret = m_orion->getRealms()->getRealm(m_realm_name, m_realm_request_generation, [this](Realm realm, orion::Error error) {
			// got a result
			m_realm_request_generation = 0;
			if (error == orion::Error::OK) {
				m_realm_resolved = true;
				m_realm = realm;

			}
			onIOComplete(error);
			});
		return ret;
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

		m_realm_resolved = true;

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
		if (m_address.has_value()) {
			const auto scheme = m_address->scheme();

			if (scheme == AddressScheme::File ||
				scheme == AddressScheme::Http ||
				scheme == AddressScheme::Https)
			{
				m_next_state = State::RESOURCE_RESOLVE;
			}
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

	void ResourceRequest::catalogReaderJson(psio::UrlResponse* resp, ResultKind kind)
	{
		const auto& path = m_effective_address->path();
		const bool is_directory_by_path = path.empty() || path.back() == '/';

		std::string mime_type;
		MIME::GetFromContentType(resp->getContentType(), mime_type);
		if (mime_type.compare("application/json") != 0) {
			// A json document is required.
			if (!is_directory_by_path) {
				// if it was a directory request, then we can fail
				if (kind == ResultKind::Catalog) {
					onIOComplete(Error::DATA_INVALID);
					return;
				}
				// may be a resource - try to read it as a resource
				m_stream = std::make_unique<UrlResponseStream>(resp);
				onIOComplete(Error::OK);
				return;
			}
			// try with index.json appended to the path
			auto address = m_effective_address.value();
			address.appendPath("index.json");
			std::string index_url = address.toString();
			Error sub_err = doRequestUrl(index_url, ResultKind::Catalog);
			return;
		}

		auto buf = std::make_shared<psio::IOBuffer>();
		resp->onEnd([this, kind, buf, resp](size_t buflen) {
			std::unique_ptr< JsonCollection> collection;
			Error err = JsonCollection::parse(buf->reader(), buflen, m_effective_address.value(), collection);
			if (err != Error::OK) {
				// if it was a directory request, then we fail
				// not implemented means it was not a catalog - try to read it as a resource
				if (kind == ResultKind::Catalog || err != Error::NOT_IMPLEMENTED) {
					onIOComplete(err);
					return;
				}
				// may be a resource - try to read it as a resource
				m_stream = std::make_unique<BufferResourceStream>(buf);
				onIOComplete(Error::OK);
				return;
			}
			m_stream = std::move(collection);
			onIOComplete(Error::OK);
			});
		resp->onError([this, buf](int error) {
			onIOComplete(translateTransportError(error));
			});

		resp->ReadAsync(buf.get());
		return;
	}

	Error ResourceRequest::doRequestUrl(const std::string& url, ResultKind kind)
	{
		m_request = new psio::UrlRequest(m_orion->getIO());

		m_request->OnResponse([this, kind](psio::UrlResponse* resp) {
			const int code = resp->GetResponseCode();
			Error err = translateHttpStatus(code);
			
			auto effective_address_result = ResourceAddress::parse(resp->getEffectiveUrl().toString());
			if (!effective_address_result.success) {
				LOG(LOG_ERROR) << "UrlResponse effective url is invalid: " << resp->getEffectiveUrl().toString();
				onIOComplete(Error::DATA_INVALID);
				return;
			}
			m_effective_address = effective_address_result.address;

			const auto& path = m_effective_address->path();
			const bool is_directory_by_path = path.empty() || path.back() == '/';

			if (err != Error::OK && (code != 403 || !is_directory_by_path)) {
				onIOComplete(err);
				return;
			}

			std::string mime_type;
			MIME::GetFromContentType(resp->getContentType(), mime_type);

			if (kind == ResultKind::Resource ||
				(kind == ResultKind::Auto && !is_directory_by_path && mime_type.compare("application/json") != 0)) {
				m_stream = std::make_unique<UrlResponseStream>(resp);
				onIOComplete(Error::OK);
				return;
			}

			// try to load as json catalog

			if (code != 200 && code != 403) {
				// A complete document is required.
				onIOComplete(Error::DATA_INVALID);
				return;
			}

			catalogReaderJson(resp, kind);			
			});
		m_request->OnError([&](int err) {
			onIOComplete(translateTransportError(err));
			});
		m_next_state = State::RESOURCE_RESOLVE_COMPLETED;

		// GET the resource
		m_request->Get(url);
		return Error::IO_PENDING;
	}

	Error ResourceRequest::doResolveResource(Error result)
	{
		if (result != Error::OK)
			return result;

		if (!m_address) {
			m_next_state = State::RESOLVE_FAILED;
			return Error::DATA_INVALID;
		}

		if (m_address->scheme() == AddressScheme::Realm && !m_realm_resolved) {
			m_next_state = State::RESOLVE_FAILED;
			return Error::REALM_NOT_FOUND;
		}

		const auto scheme = m_address->scheme();

		if (scheme == AddressScheme::Http ||
			scheme == AddressScheme::Https)
		{
			return doRequestUrl(m_address->toString(), m_result_kind);
		}

		if (m_local) {
			// if we could not find local resource with resolvers
			// report it as missing
			m_next_state = State::UNSET;
			return Error::RESOURCE_NOT_FOUND;
		}

		if (m_address.value().scheme() == orion::AddressScheme::File) {
#if defined(_WIN32)
			if (m_realm.getType() == Realm::RealmType::FileSystem && m_path == "/") {
				m_stream = std::make_unique<FileCollection>(psio::Fs::getLogicalDrives());
				m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
				m_effective_address = m_address;
				return Error::OK;
			}
#endif
			std::string native_path;
			const Error error = orion::fileAddressToNativePath(*m_address, native_path);
			if (error != Error::OK) {
				m_next_state = State::RESOLVE_FAILED;
				return error;
			}

			m_stream = FileResourceStream::getStream(native_path);
			m_stream->setLogicalAddress(m_address->toString());
			if (!m_stream) {
				m_next_state = State::UNSET;
				return Error::RESOURCE_NOT_FOUND;
			}
			m_effective_address = m_address;
			m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
			return Error::OK;
		}

		// realm address - we need to resolve the realm first
		if (m_realm.getType() != Realm::RealmType::Unset) {
			std::string path;

			if (m_realm.isLocalPath() || m_realm.getType() == Realm::RealmType::FileSystem) {
#if defined(_WIN32)
				if (m_realm.getType() == Realm::RealmType::FileSystem && m_path == "/") {
					m_stream = std::make_unique<FileCollection>(psio::Fs::getLogicalDrives());
					m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
					AddressResult effective_address_result = orion::ResourceAddress::parse("file:///");
					m_effective_address = effective_address_result.address;
					return Error::OK;
				}
#endif
				std::string native_path;
				const Error error = m_realm.resolveFilePath(m_path, native_path);
				if (error != Error::OK) {
					m_next_state = State::UNSET;
					return error;
				}

				m_stream = FileResourceStream::getStream(native_path);
				m_stream->setLogicalAddress(m_address->toString());
				if (!m_stream) {
					m_next_state = State::UNSET;
					return Error::RESOURCE_NOT_FOUND;
				}
				if (native_path.size() && native_path[0] != '/') {
					// if the path is not rooted we need to add a leading slash for the effective address
					native_path = "/" + native_path;
				}
				AddressResult effective_address_result = orion::ResourceAddress::parse("file://" + native_path);
				if (!effective_address_result.success) {
					LOG(LOG_ERROR) << "Effective address is invalid: " << "file://" + native_path << "; reason: " << effective_address_result.error.message;
					m_next_state = State::UNSET;
					return Error::DATA_INVALID;
				}
				m_effective_address = effective_address_result.address;
				m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
				return Error::OK;
			}

			path = m_realm.getResourceUrl();
			if (path == "") {
				return Error::DATA_INVALID;
			}
			if (path.back() == '/' && m_path[0] == '/')
				path += m_path.substr(1);
			else
				path += m_path;

			// Request the resource
			AddressResult effective_address_result = orion::ResourceAddress::parse(path);
			m_effective_address = effective_address_result.address;
			return doRequestUrl(path);
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
