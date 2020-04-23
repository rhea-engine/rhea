#include "Orion.h"

#include "ResourceCache.h"
#include "ResourceRequest.h"
#include "ResourceStream.h"
#include "FileResource.h"
#include "Realm.h"

namespace Orion
{	
	Orion::Orion(string appname)
	{
		m_cache = NULL;
		m_realms = NULL;
		m_appname = appname;
		m_fs = new Fs(appname);
	}

	ResourceRequest* Orion::getResourceByUrl(const string url)
	{
		// get the file via resource cache
		if (!m_cache) {
			m_cache = new ResourceCache();
		}

		

		return NULL;
	}

	ResourceRequest* Orion::getResourceByRid(const string rid)
	{
		ResourceRequest* res_id;
		
		// parse the rid and try to resolve it
		res_id = new ResourceRequest(rid, this);
		return res_id;		
	}

	Realms* Orion::getRealms()
	{
		if (!m_realms)
			m_realms = new Realms(this);

		return m_realms;
	}

	void Orion::setAppDir(std::string dirname)
	{
		m_fs->setAppDataDir(dirname);
	}

	Orion::~Orion()
	{
		if (m_cache)
			delete m_cache;
		if (m_realms)
			delete m_realms;
	}
}