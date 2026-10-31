/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_RESOURCEREQUEST_H
#define ORION_RESOURCEREQUEST_H

#include <string>
#include <functional>
#include <optional>

#include <Orion/ResourceAddress.h>
#include <Orion/Realm.h>
#include <Psio/Net/UrlRequest/UrlRequest.h>
#include <Orion/RequestError.h>
#include <memory>

namespace orion
{
	class Orion;
	class ResourceStream;
	class ResourceReadyInterface;

	class ResourceRequestInfo
	{
	protected:
	};

	class ResourceRequest
	{
	public:

		typedef std::function<void(ResourceRequest*, orion::Error)> RequestHandler;
		ResourceRequest() = default;
		ResourceRequest(const ResourceAddress& address,	Orion* orion, std::string mime_type = "");
		ResourceRequest(std::string rid, Orion* orion, std::string mime_type = "");
		~ResourceRequest();

		// get realm name
		std::string getRealmName() { return m_realm_name; }
		Realm* getRealm() { return &m_realm; }

		// get full path without realm
		std::string getPath() { return m_path; }

		// get parent path without realm
		std::string getParentPath();

		// get base name of request
		std::string getBasename() { return m_basename; }

		// address of the resource
		std::optional<ResourceAddress> getAddress() { return m_address; }
		void setAddress(ResourceAddress address);

		bool isResolved() { return m_resolved; }
		ResourceStream* getStream() { return m_stream.get(); }
		void setMimeType(std::string type) { m_mime_type = type; }
		void setMimeTypeFromExtension();
		std::string getMimeType();

		// status info functions
		bool isReading();
		bool isWriting();
		bool isFailed() { return m_state == State::RESOLVE_FAILED; }
		bool isCompleted();

		// resolve the realm
		// calls orion::onRealmResolved with itself as parameter
		// return 0 if resolver started, negative on error
		Error resolveRealm(Orion* orion);

		// resolve the resource
		// on ready call the callback handler
		// return 0 if resolver started, negative on error
		Error resolve(RequestHandler handler);

		// set a different handler
		void setHandler(RequestHandler handler) { m_handler = handler; };

	protected:

		enum class State {
			UNSET,
			CACHE_RESOLVE,
			REALM_RESOLVE,
			REALM_RESOLVE_COMPLETED,			
			RESOURCE_RESOLVE,
			RESOURCE_RESOLVE_COMPLETED,
			RESOLVE_FAILED
		};

		State m_state = ResourceRequest::State::UNSET;
		State m_next_state = ResourceRequest::State::UNSET;

		Error ResolveLoop(Error result);
		Error doCacheResolve(Error result);
		Error doResolveResource(Error result);
		Error doResolveResourceCompleted(Error result);
		Error doResolveRealm(Error result);
		Error doResolveRealmCompleted(Error result);
		Error doResolveResourceFailed(Error result);
		Error doRequestUrl(const std::string& url);

		std::optional<ResourceAddress> m_address;

		std::string m_realm_name;
		std::string m_path;
		std::string m_container_path;
		std::string m_basename;
		Realm m_realm;
		bool m_resolved = false;
		bool m_realm_resolved = false;
		std::unique_ptr<ResourceStream> m_stream;
		Error m_error = Error::OK;

		int m_request_type = 0;

		Orion* m_orion = nullptr;

		// MIME type
		std::string m_mime_type;

		// wheter realm is a local realm
		bool m_local = false;

		RequestHandler m_handler;

		psio::UrlRequest* m_request = nullptr;

		// IO step
		void onIOComplete(Error result);
	};
}

#endif // !ORION_RESOURCEREQUEST_H
