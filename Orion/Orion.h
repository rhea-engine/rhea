#ifndef ORION_H
#define ORION_H

#include <vector>
#include <string>
#include <functional>

using std::string;

#include "Collection.h"
#include "os/Fs.h"
#include "Realms.h"
#include <Psio/Psio.h>
#include "ResolverInterface.h"

namespace Orion
{	
	class ResourceCache;
	class ResourceRequest;

	class Orion
	{
	public:
		Orion(Psio::Engine* engine, string appname);
		~Orion();

		// Poll the requests
		bool poll();

		// if we have the url for a resource get it
		ResourceRequest* getResourceByUrl(const string url);
		
		// if we have the rid we have to resolve the Resource and then get it
		ResourceRequest* getResourceByRid(const string rid);

		// get by cache
		std::unique_ptr<ResourceStream> getResourceFromCache(const string realm_name, const string resource_path);

		// force closing a request
		void close(ResourceRequest* req);

		// callback for resolved resource
		int resolve(string rid, ResourceRequest** resourceId);		
		// Fs getter
		Fs* getFs() { return m_fs; }

		// psio getter
		Psio::Engine* getIO() { return m_engine; }

		// get realm from realm list
		Realms* getRealms();		

		// set an app dir manually
		void setAppDir(std::string dirname);

		// set a local dir manually
		void setLocalDir(std::string dirname);

		static const int ERR_NOTFOUND = -1;
		static const int ERR_NOTIMPLEMENTED = -2;

	protected:
		// realm list
		std::unique_ptr<Realms> m_realms;

		// the resource cache
		std::vector<std::unique_ptr<ResolverInterface>> m_caches;

		// pointer to the OS Fs
		Fs* m_fs;

		// the appname is used for default paths
		string m_appname;

		// the input output engine - used for network
		Psio::Engine* m_engine;

		// list of current read requests
		std::vector<ResourceRequest*> m_requests;
	};
}

#endif // !ORION_H
