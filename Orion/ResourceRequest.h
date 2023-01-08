#ifndef ORION_RESOURCEREQUEST_H
#define ORION_RESOURCEREQUEST_H

#include <string>

#include "Realm.h"
#include <Psio/Net/UrlRequest/UrlRequest.h>

using std::string;

namespace Orion
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
		enum Error {
			OK = 0,
			IO_PENDING = -1,
			REALM_NOT_FOUND = -2,
			RESOURCE_NOT_FOUND = -3,
			NOT_IMPLEMENTED = -4,
			PATH_NOT_FOUND = -5,
			NO_AUTH = -6,
			NO_ACCESS = -7,
			CONFIG_ERR = -8,
			DATA_INVALID = -9
		};

		typedef std::function<void(ResourceRequest*, int err)> RequestHandler;

		ResourceRequest(string rid, Orion *orion);
		ResourceRequest(string rid, Orion* orion, std::string type);
		~ResourceRequest();

		// get realm name
		string getRealmName() { return m_realm_name; }
		Realm* getRealm() { return m_realm; }

		// get full path without realm
		string getPath() { return m_path; }

		// get parent path without realm
		string getParentPath();

		// get base name of request
		string getBasename();

		bool isResolved() { return m_resolved; }
		Psio::Stream* getStream() { return m_stream; }
		void setMimeType(std::string type) { m_mime_type = type; }
		std::string getMimeType();

		// status info functions
		bool isReading();
		bool isWriting();

		// parse a resource identifier
		// rid is //realm/container/node/subnode/...
		// where container normally is an owner (but could be a directory)
		// the resolver does only lookup the realm, the rest is considered to be a path, where the server is responsible
		// special realm is "local"
		// return 0 on success, negative on error
		Error ResourceRequest::parseRid(const string rid);

		// resolve the realm
		// calls orion::onRealmResolved with itself as parameter
		// return 0 if resolver started, negative on error
		Error resolveRealm(Orion* orion);

		// resolve the resource
		// on ready call the callback handler
		// return 0 if resolver started, negative on error
		int resolve(RequestHandler handler);

		// set a different handler
		void setHandler(RequestHandler handler) { m_handler = handler; };

		// get an english error message for the error
		static std::string getErrorMessage(Error err);

	protected:

		enum class State {
			UNSET,
			REALM_RESOLVE,
			REALM_RESOLVE_COMPLETED,			
			RESOURCE_RESOLVE,
			RESOURCE_RESOLVE_COMPLETED,
			RESOLVE_FAILED
		};

		State m_state = ResourceRequest::State::UNSET;
		State m_next_state = ResourceRequest::State::UNSET;

		int ResolveLoop(int result);
		int doResolveResource(int result);
		int doResolveResourceCompleted(int result);
		int doResolveRealm(int result);
		int doResolveRealmCompleted(int result);
		int doResolveResourceFailed(int result);

		string m_realm_name;
		string m_path;
		string m_container_path;
		string m_basename;
		Realm* m_realm = NULL;
		bool m_resolved = false;
		bool m_realm_resolved = false;
		Psio::Stream* m_stream = NULL;
		Error m_error = Error::OK;

		int request_type = 0;

		Orion* m_orion = NULL;

		// MIME type
		std::string m_mime_type;

		// wheter realm is a local realm
		bool m_local = false;

		// wheter the realm is an addon (installed with program)
		bool m_addon = false;	
		RequestHandler m_handler;

		Psio::UrlRequest* m_request;

		// IO step
		void onIOComplete(int result);
	};

}

#endif // !ORION_RESOURCEREQUEST_H
