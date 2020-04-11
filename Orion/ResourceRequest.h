#ifndef ORION_RESOURCEREQUEST_H
#define ORION_RESOURCEREQUEST_H

#include <string>

#include "Realm.h"
#include "IOCompleteHandler.h"

using std::string;

namespace Orion
{
	class Orion;
	class Resource;
	class ResourceReadyInterface;

	class ResourceRequestInfo
	{
	protected:
	};

	class ResourceRequest : public IOCompleteHandler
	{
	public:
		ResourceRequest(string realm, string path);
		ResourceRequest(string rid);

		string getRealmName() { return m_realm_name; };
		string getPath() { return m_path; };
		bool isResolved() { return m_resolved; };

		enum class Error {
			OK,
			IO_PENDING,
			REALM_NOT_FOUND,
			RESOURCE_NOT_FOUND,
			NOT_IMPLEMENTED
		};

		// set the onresource ready callback
		void setOnResourceReady(ResourceReadyInterface* callback)
		{
			m_ready_callback = callback;
		}

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
		// precondition: realm is resolved
		// on ready call the onReady callback
		// return 0 if resolver started, negative on error
		Error resolve(Orion* orion);

		void onEvent(Event* e) {};
		void onIOComplete(Event* e);

		

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

		Error doResolve();
		Error doResolveResource();
		Error doResolveResourceCompleted();
		Error doResolveRealm();
		Error doResolveRealmCompleted();
		Error doResolveResourceFailed();

		string m_realm_name;
		string m_path;
		Realm* m_realm = NULL;
		bool m_resolved = false;
		bool m_realm_resolved = false;
		Resource* m_resource = NULL;
		Error m_error = Error::OK;

		int request_type = 0;

		Orion* m_orion = NULL;

		// wheter realm is a local realm
		bool m_local = false;

		// wheter the realm is an addon (installed with program)
		bool m_addon = false;
		ResourceReadyInterface* m_ready_callback = 0;		
	
	};

}

#endif // !ORION_RESOURCEREQUEST_H
