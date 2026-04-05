#include <Orion/Orion.h>

#include <Orion/ResourceCache.h>
#include <Orion/ResourceRequest.h>
#include <Orion/ResourceStream.h>
#include <Orion/Realm.h>
#include <Orion/ResolveLocal.h>

namespace orion
{	
	Orion::Orion(psio::Engine* engine, std::string appname)
	{
		m_appname = appname;
		m_fs = engine->getFs();
		m_fs->setAppName(appname);
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
			m_caches.push_back(std::make_unique<ResolveLocal>(search_dir));
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

	ResourceRequest* Orion::getResourceByUrl(const std::string url)
	{
		// get the file via resource cache
		

		

		return NULL;
	}

	ResourceRequest* Orion::getResourceByRid(const std::string rid)
	{
		ResourceRequest* res_id;
		
		// parse the rid and try to resolve it
		res_id = new ResourceRequest(rid, this);
		m_requests.push_back(res_id);
		return res_id;		
	}

	std::unique_ptr<ResourceStream> Orion::getResourceFromCache(const std::string realm_name, const std::string resource_path)
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

	bool Orion::checkResourceFromCache(const std::string realm_name, const std::string resource_path)
	{
		// try resolvers
		
		for (auto it = m_caches.begin(); it != m_caches.end(); ++it)
		{
			if ((*it)->checkResourceExists(realm_name, resource_path))
				return true;
		}

		// not found in caches
		return false;
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