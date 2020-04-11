#ifndef ORION_H
#define ORION_H

#include <vector>
#include <string>

using std::string;

#include "Resource.h"
#include "Collection.h"
#include "os/Fs.h"
#include "Realms.h"

namespace Orion
{	
	class ResourceCache;
	class ResourceRequest;
	class ResourceReadyInterface;

	class Orion
	{
	protected:
		// realm list
		Realms* m_realms;

		// the reosurce cache
		ResourceCache* m_cache;
		// pinter to the OS Fs
		Fs* m_fs;
		// the appname is used for default paths
		string m_appname;
	public:
		Orion(string appname);
		~Orion();
		// if we have the url for a resource get it
		int getResourceByUrl(const string url, ResourceReadyInterface* callback);
		// if we have the rid we have to reolve the Resource and then get it
		int getResourceByRid(const string rid, ResourceReadyInterface* callback);
		// callback for resolved resource
		int resolve(string rid, ResourceRequest** resourceId);
		// callback for resolved realm
		void onRealmResolved(ResourceRequest* res_id);
		// Fs getter
		Fs* getFs() { return m_fs; }

		// get realm from realm list
		Realms* getRealms();		

		static const int ERR_NOTFOUND = -1;
		static const int ERR_NOTIMPLEMENTED = -2;
	};
}

#endif // !ORION_H
