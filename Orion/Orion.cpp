#include "Orion.h"

#include "ResourceCache.h"
#include "ResourceRequest.h"
#include "ResourceStream.h"
#include "Realm.h"
#include "ResolveLocal.h"

namespace Orion
{	
	Orion::Orion(Psio::Engine* engine, string appname)
	{
		m_appname = appname;
		m_fs = new Fs(appname);
		m_engine = engine;

		std::string search_dir;

		search_dir = m_fs->getInstallDataDir();
		if (search_dir != "")
			m_caches.push_back(std::make_unique<ResolveLocal>(search_dir));
		search_dir = m_fs->getAppDataDir();
		if (search_dir != "")
			m_caches.push_back(std::make_unique<ResolveLocal>(search_dir));
		search_dir = m_fs->getHomeDir();
		if (search_dir != "")
			m_caches.push_back(std::make_unique<ResolveLocal>(search_dir, true));
	}

	bool Orion::poll()
	{
		for (auto it = m_requests.begin(); it != m_requests.end(); ++it)
		{
			ResourceRequest* req = *it;
			if (req->isReading()) {
				// read
			}
				
			if (req->isWriting()) {

			}
		}

		return false;
	}

	ResourceRequest* Orion::getResourceByUrl(const string url)
	{
		// get the file via resource cache
		

		

		return NULL;
	}

	ResourceRequest* Orion::getResourceByRid(const string rid)
	{
		ResourceRequest* res_id;
		
		// parse the rid and try to resolve it
		res_id = new ResourceRequest(rid, this);
		m_requests.push_back(res_id);
		return res_id;		
	}

	std::unique_ptr<ResourceStream> Orion::getResourceFromCache(const string realm_name, const string resource_path)
	{
		// try resolvers
		std::unique_ptr<ResourceStream> stream;

		for (auto it = m_caches.begin(); it != m_caches.end(); ++it)
		{
			stream = (*it)->getResource(realm_name, resource_path);
			if (stream)
				return stream;
		}

		// not found in caches
		return nullptr;
	}

	Realms* Orion::getRealms()
	{
		if (!m_realms)
			m_realms = std::make_unique<Realms>(this);
		
		return m_realms.get();
	}

	void Orion::setAppDir(std::string dirname)
	{
		m_caches.push_back(std::make_unique<ResolveLocal>(dirname));
	}

	void Orion::setLocalDir(std::string dirname)
	{
		m_caches.push_back(std::make_unique<ResolveLocal>(dirname, true));
	}

	Orion::~Orion()
	{		
	}
}