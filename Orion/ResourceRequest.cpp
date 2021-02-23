#include "Orion.h"
#include "ResourceRequest.h"
#include "FileResource.h"

#include <iostream>

namespace Orion
{	
	ResourceRequest::ResourceRequest(string rid, Orion *orion)
	{
		m_realm_name = "";
		m_path = "";
		m_orion = orion;
		parseRid(rid);
	}

	ResourceRequest::ResourceRequest(string rid, Orion* orion, std::string type)
	{
		m_type = type;
		m_realm_name = "";
		m_path = "";
		m_orion = orion;
		parseRid(rid);
	}

	string ResourceRequest::getParentPath()
	{
		return m_container_path;
	}

	string ResourceRequest::getBasename()
	{
		return m_basename;
	}

	std::string ResourceRequest::getErrorMessage(Error err)
	{
		switch (err) {
		case Error::OK:
			return "OK";
		case Error::IO_PENDING:
			return "Data is pending";
		case Error::REALM_NOT_FOUND:
			return "Realm not found";
		case Error::RESOURCE_NOT_FOUND:
			return "Resource not found";
		case Error::NO_ACCESS:
			return "Permission denied";
		case Error::NO_AUTH:
			return "Authentication failed";
		default:
			return "General error";
		}
	}
	
	ResourceRequest::Error ResourceRequest::parseRid(const string rid)
	{
		size_t pathpos;
		size_t realmpos = 2;

		// make sure the rid is in the correct form
		if (rid.compare(0, 2, "//") && rid.compare(0, 2, "\\\\"))
		{
			// no - try to use full string as realm
			realmpos = 0;
		}

		// split the realm part
		pathpos = rid.find_first_of("\\/", realmpos);
		if (pathpos != std::string::npos)
		{
			m_realm_name = rid.substr(realmpos, pathpos-realmpos);
			m_path = rid.substr(pathpos);
			// find basename
			size_t basepos = m_path.find_last_of("\\/");
			if (basepos != std::string::npos) {
				m_basename = m_path.substr(basepos + 1);
				m_container_path = m_path.substr(0, basepos);
			}
		}
		else {
			// no path, only realm
			m_realm_name = rid.substr(realmpos);
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

	ResourceRequest::Error ResourceRequest::resolve(RequestHandler handler)
	{
		m_next_state = State::REALM_RESOLVE;		
		m_handler = handler;
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

			path += string("/") + m_path;		

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
		m_handler(NULL, Error::NOT_IMPLEMENTED);
		
		m_next_state = State::UNSET;
		return Error::NOT_IMPLEMENTED;
	}

	ResourceRequest::Error ResourceRequest::doResolveResourceCompleted()
	{
		// call callbacks if any
		m_next_state = State::UNSET;

		if (m_handler) {
			m_handler(this, Error::OK);
		}
		
		return Error::OK;
	}

	ResourceRequest::Error ResourceRequest::doResolveResourceFailed()
	{
		if (m_handler) {
			m_handler(NULL, m_error);
		}
		
		// TODO call callbacks if any
		m_next_state = State::UNSET;

		return Error::OK;
	}
}