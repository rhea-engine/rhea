#include "Orion.h"
#include "ResourceRequest.h"
#include "ResourceReadyInterface.h"
#include "FileResource.h"

#include <iostream>

namespace Orion
{
	ResourceRequest::ResourceRequest(string realm, string path)
	{
		m_realm_name = realm;
		m_path = path;
	}

	ResourceRequest::ResourceRequest(string rid)
	{
		m_realm_name = "";
		m_path = "";
		parseRid(rid);
	}
	
	ResourceRequest::Error ResourceRequest::parseRid(const string rid)
	{
		size_t pathpos;
		size_t realmpos = 2;
		ResourceRequest* resId;

		// make sure the rid is in the correct form
		if (rid.compare(0, 2, "//") && rid.compare(0, 2, "\\\\"))
		{
			// no - try to use full string as realm
			realmpos = 0;
		}

		// split the realm part
		if (pathpos = rid.find("/", realmpos))
		{
			m_realm_name = rid.substr(realmpos, pathpos-realmpos);
			m_path = rid.substr(pathpos, string::npos);
		}
		else {
			// no path, only realm
			m_realm_name = rid.substr(realmpos, string::npos);
			m_path = "";
		}

		// if realm is empty use 'local'
		if (m_realm_name.empty())
			m_realm_name = "local";

		return Error::OK;
	}
	
	ResourceRequest::Error ResourceRequest::doResolve()
	{
		m_state = m_next_state;
		do
		{
			m_state = m_next_state;

			switch (m_state)
			{
			case ResourceRequest::State::UNSET:
				break;
			case ResourceRequest::State::REALM_RESOLVE:
				m_error = doResolveRealm();
				break;
			case ResourceRequest::State::REALM_RESOLVE_COMPLETED:
				m_error = doResolveRealmCompleted();
				break;
			case ResourceRequest::State::RESOURCE_RESOLVE:
				m_error = doResolveResource();
				break;
			case ResourceRequest::State::RESOURCE_RESOLVE_COMPLETED:
				m_error = doResolveResourceCompleted();
				break;
			case ResourceRequest::State::RESOLVE_FAILED:
				m_error = doResolveResourceFailed();
			default:
				break;
			}

		} while ((m_error != Error::IO_PENDING) && (m_state != State::UNSET));

		return m_error;
	}

	ResourceRequest::Error ResourceRequest::resolveRealm(Orion* orion)
	{
		m_orion = orion;
		return Error::OK;
	}

	ResourceRequest::Error ResourceRequest::doResolveRealm()
	{
		m_next_state = State::REALM_RESOLVE_COMPLETED;

		// if realm already resolved do nothing
		if (m_realm_resolved) {
			return Error::OK;
		}

		// local realm is always resolved
		if (!m_realm_name.compare("local")) {
			m_local = true;
			m_realm_resolved = true;
			return Error::OK;
		}

		// check if the realm is provided by the installation
		// this is used for core realms and addons
		string path;

		path = m_orion->getFs()->getInstallDataDir();
		path += string("/") + m_realm_name;
		if (Fs::fileExists(path))
		{
			m_addon = true;
			m_realm_resolved = true;			
			return Error::OK;
		}

		// try to get cached value
		Realm* r = m_orion->getRealms()->getRealmCached(m_realm_name);
		if (r) {
			m_realm_resolved = true;
			m_realm = r;
			return Error::OK;
		}

		// get data from realm list
		if (!m_orion->getRealms()->getRealm(m_realm_name, this))
			return Error::OK;
		else
			return Error::IO_PENDING;
	}

	ResourceRequest::Error ResourceRequest::doResolveRealmCompleted()
	{
		//  get the realm if not resolved already 
		if (!m_realm_resolved) {
			Realm* r = m_orion->getRealms()->getRealmCached(m_realm_name);
			if (!r) {
				// if we cant find the realm in the cache the lookup failed
				// fail permanently
				m_next_state = State::RESOLVE_FAILED;
				return Error::REALM_NOT_FOUND;
			}
			m_realm = r;
		}

		// TODO call callbacks if any
		m_next_state = State::RESOURCE_RESOLVE;
		return Error::OK;
	}

	ResourceRequest::Error ResourceRequest::resolve(Orion* orion)
	{
		m_next_state = State::REALM_RESOLVE;
		m_orion = orion;
		return doResolve();
	}

	void ResourceRequest::onIOComplete(Event* e)
	{
		doResolve();
	}

	ResourceRequest::Error ResourceRequest::doResolveResource()
	{
		// safety check
		if (!m_realm_resolved)
			return Error::IO_PENDING;

		// local files are directly usable
		if (m_local) {
			m_resolved = true;
			
			// open the resource as a file resource

			// resolve the path
			string path;
			path = m_orion->getFs()->getAppDataDir();

			path += string("/") + m_realm_name + m_path;		

			if (Fs::fileExists(path))
			{
				// file exists
				// create file resource and call callback
				m_resource = (Resource*)new FileResource(path);
				m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
				return Error::OK;
			}
			else
			{
				// does not exists
				m_next_state = State::RESOLVE_FAILED;
				return Error::RESOURCE_NOT_FOUND;
			}
		}

		// check if realm is provided by installation
		// these are directly usable
		if (m_addon)
		{
			string path;

			path = m_orion->getFs()->getInstallDataDir();
			path += string("/") + m_realm_name + m_path;

			if (Fs::fileExists(path))
			{
				// file exists
				// create file resource and set next state
				m_resource = (Resource*)new FileResource(path);
				m_next_state = State::RESOURCE_RESOLVE_COMPLETED;
				return Error::OK;
			}
			else {
				// does not exists
				m_next_state = State::RESOLVE_FAILED;
				return Error::RESOURCE_NOT_FOUND;
			}
		}

		// not implemented
		// call callback with error method
		m_ready_callback->onResourceFailed(Error::NOT_IMPLEMENTED);
		
		m_next_state = State::UNSET;
		return Error::NOT_IMPLEMENTED;
	}

	ResourceRequest::Error ResourceRequest::doResolveResourceCompleted()
	{
		// TODO call callbacks if any
		m_next_state = State::UNSET;

		m_ready_callback->onResourceReady(m_resource);

		return Error::OK;
	}

	ResourceRequest::Error ResourceRequest::doResolveResourceFailed()
	{
		m_ready_callback->onResourceFailed(m_error);

		// TODO call callbacks if any
		m_next_state = State::UNSET;

		return Error::OK;
	}
}