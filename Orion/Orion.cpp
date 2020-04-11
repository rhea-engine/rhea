#include "Orion.h"

#include "ResourceCache.h"
#include "ResourceRequest.h"
#include "ResourceReadyInterface.h"
#include "ResourceStream.h"
#include "FileResource.h"
#include "Realm.h"

namespace Orion
{	
	Orion::Orion(string appname)
	{
		m_cache = 0;
		m_appname = appname;
		m_fs = new Fs(appname);
	}

	int Orion::getResourceByUrl(const string url, ResourceReadyInterface* callback)
	{
		// get the file via resource cache
		if (!m_cache) {
			m_cache = new ResourceCache();
		}

		

		return NULL;
	}

	void Orion::onRealmResolved(ResourceRequest* res_id)
	{
		res_id->resolve(this);
	}


	int Orion::getResourceByRid(const string rid, ResourceReadyInterface* callback)
	{
		ResourceRequest* res_id;
		
		// parse the rid and try to resolve it
		res_id = new ResourceRequest(rid);
		res_id->setOnResourceReady(callback);
		if (res_id->resolve(this) == ResourceRequest::Error::OK)
			return 0;
		else
			return -1;
	}
	
	Realms* Orion::getRealms()
	{
		if (!m_realms)
			m_realms = new Realms(this);

		return m_realms;
	}

	Orion::~Orion()
	{
		if (m_cache)
			delete m_cache;
		if (m_realms)
			delete m_realms;
	}
}