#ifndef ORION_H
#define ORION_H

#include <vector>
#include <string>
#include <functional>

using std::string;

#include "Resource.h"
#include "Collection.h"
#include "os/Fs.h"
#include "Realms.h"

namespace Orion
{	
	class ResourceCache;
	class ResourceRequest;

	class Orion
	{
	public:
		Orion(string appname);
		~Orion();

		// if we have the url for a resource get it
		ResourceRequest* getResourceByUrl(const string url);
		// if we have the rid we have to reolve the Resource and then get it
		ResourceRequest* getResourceByRid(const string rid);
		// callback for resolved resource
		int resolve(string rid, ResourceRequest** resourceId);		
		// Fs getter
		Fs* getFs() { return m_fs; }

		// get realm from realm list
		Realms* getRealms();		

		// set the app dir manually
		void setAppDir(std::string dirname);

		static const int ERR_NOTFOUND = -1;
		static const int ERR_NOTIMPLEMENTED = -2;

	protected:
		// realm list
		Realms* m_realms;

		// the resource cache
		ResourceCache* m_cache;

		// pointer to the OS Fs
		Fs* m_fs;

		// the appname is used for default paths
		string m_appname;

	};
}

#endif // !ORION_H
