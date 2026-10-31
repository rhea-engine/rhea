/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/Orion.h>

#include <Orion/ResourceCache.h>
#include <Orion/ResourceRequest.h>
#include <Orion/ResourceStream.h>
#include <Orion/Realm.h>
#include <Orion/ResolveLocal.h>
#include <Orion/Realms.h>
#include <Orion/FileAddress.h>

namespace orion
{	
	Orion::Orion(psio::Engine* engine, std::string appname)
	{
		m_appname = appname;
		m_fs = engine->getFs();
		m_fs->setAppName(appname);
		m_engine = engine;
		m_realms = std::make_unique<Realms>(this);

		std::string search_dir;

		search_dir = m_fs->getInstallDataDir();
		if (search_dir != "") {
			m_caches.push_back(std::make_unique<ResolveLocal>(search_dir));
			m_realms->addRealm(Realm("system", search_dir, Realm::RealmType::LocalPath));
		}
		search_dir = m_fs->getAppDataDir();
		if (search_dir != "") {
			m_caches.push_back(std::make_unique<ResolveLocal>(search_dir));
			m_realms->addRealm(Realm("appdata", search_dir, Realm::RealmType::LocalPath));
		}
		search_dir = m_fs->getHomeDir();		
		if (search_dir != "") {
			m_caches.push_back(std::make_unique<ResolveLocal>(search_dir));
			m_realms->addRealm(Realm("user", search_dir, Realm::RealmType::LocalPath));
		}
		// add filesystem realm for direct access to local files
		m_realms->addRealm(Realm("file", "", Realm::RealmType::FileSystem));
	}

	void Orion::addLocalRealm(std::string name, std::string path)
	{
		m_realms->addRealm(Realm(name, path, Realm::RealmType::LocalPath));
	}

	std::string Orion::getOrCreateProjectRealm(const ResourceAddress& physical_directory)
	{
		const std::string path = physical_directory.path();

		// requires a validated directory address ending in '/'.
		if (path.empty() || path.back() != '/' || physical_directory.toString().find('?') != std::string::npos)
			return {};
		const std::string key = physical_directory.toString();

		auto existing = m_project_realms.find(key);
		if (existing != m_project_realms.end())
			return existing->second;

		Realm::RealmType type;
		std::string location;

		switch (physical_directory.scheme()) {
		case AddressScheme::File:
			type = Realm::RealmType::LocalPath;

			if (orion::fileAddressToNativePath(physical_directory, location) != Error::OK) {
				return {};
			}
			break;

		case AddressScheme::Http:
		case AddressScheme::Https:
			type = Realm::RealmType::URL;
			location = physical_directory.toString();
			break;

		default:
			return {};
		}

		std::string name;
		Realm existing_realm;

		do {
			name = "project-realm-" + std::to_string(m_next_project_realm++);
		} while (
			m_realms->getRealmCached(name, existing_realm) == Error::OK);

		Realm realm(name, location, type);
		m_realms->addProjectRealm(realm);
		m_project_realms.emplace(key, name);

		return name;
	}

	bool Orion::poll()
	{
		for (auto it = m_requests.begin(); it != m_requests.end(); ++it)
		{
			ResourceRequest* req = it->get();
			if (req->isCompleted() || req->isFailed())
			{
				// remove request from list
				m_requests.erase(it);
				return true;
			}
		}

		return false;
	}

	ResourceRequest* Orion::getResourceByUrl(const std::string url)
	{
		// get the file via resource cache
		

		

		return NULL;
	}

	ResourceRequest* Orion::getResourceByRid(const std::string& rid, const std::string& base_address)
	{
		std::unique_ptr<ResourceRequest> res_id;
		
		// parse the rid and try to resolve it
		res_id = std::make_unique<ResourceRequest>(rid, this, "", base_address);
		m_requests.push_back(std::move(res_id));
		return m_requests.back().get();
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
		// remove old local dir resolver if it exists
		for (auto it = m_caches.begin(); it != m_caches.end(); ++it) {
			ResolveLocal* resolver = static_cast<ResolveLocal*>(it->get());
			if (resolver && resolver->getBasicAddress() == m_local_dir) {
				m_caches.erase(it);
				m_local_dir.clear();
				break;
			}
		}

		if (!dirname.empty()) {
			m_caches.push_back(std::make_unique<ResolveLocal>(dirname, true));
			m_local_dir = dirname;
		}
	}

	bool Orion::hasLocalDir()
	{
		return !m_local_dir.empty();
	}

	Orion::~Orion()
	{		
	}
}