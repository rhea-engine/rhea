/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_H
#define ORION_H

#include <vector>
#include <string>
#include <functional>

#include <Orion/Collection.h>
#include <Psio/Fs/Fs.h>
#include <Orion/Realms.h>
#include <Psio/Psio.h>
#include <Orion/ResolverInterface.h>

namespace orion
{	
	class ResourceCache;
	class ResourceRequest;

	class Orion
	{
	public:
		Orion(psio::Engine* engine, std::string appname);
		~Orion();

		// Poll the requests
		bool poll();

		// if we have the url for a resource get it
		ResourceRequest* getResourceByUrl(const std::string url);
		
		// if we have the rid we have to resolve the Resource and then get it
		ResourceRequest* getResourceByRid(const std::string& rid, const std::string& base_address = {});

		// get by cache
		std::unique_ptr<ResourceStream> getResourceFromCache(const std::string realm_name, const std::string resource_path);

		// check in cache
		bool checkResourceFromCache(const std::string realm_name, const std::string resource_path);

		// force closing a request
		void close(ResourceRequest* req);

		// callback for resolved resource
		int resolve(std::string rid, ResourceRequest** resourceId);		
		// Fs getter
		psio::Fs* getFs() { return m_fs; }

		// psio getter
		psio::Engine* getIO() { return m_engine; }

		// get realm from realm list
		Realms* getRealms();		

		// set an app dir manually
		void setAppDir(std::string dirname);

		// set a local dir manually
		void setLocalDir(std::string dirname);

		// do we have a local dir
		bool hasLocalDir();

		// set a custom realm
		void addLocalRealm(std::string name, std::string path);

		static const int ERR_NOTFOUND = -1;
		static const int ERR_NOTIMPLEMENTED = -2;

	protected:
		// realm list
		std::unique_ptr<Realms> m_realms;

		// the resource cache
		std::vector<std::unique_ptr<ResolverInterface>> m_caches;

		// pointer to the OS Fs
		psio::Fs* m_fs;

		// the appname is used for default paths
		std::string m_appname;

		// the input output engine - used for network
		psio::Engine* m_engine;

		// list of current read requests
		std::vector<std::unique_ptr<ResourceRequest>> m_requests;

		// do we have a local dir
		std::string m_local_dir;
	};
}

#endif // !ORION_H
