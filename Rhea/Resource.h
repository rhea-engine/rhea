#ifndef RHEA_RESOURCE_H
#define RHEA_RESOURCE_H

#include <functional>

#include "Orion/ResourceRequest.h"

#include "Unit.h"

using ReqError = Orion::ResourceRequest::Error;

namespace Rhea
{
	// a resource is a unit that holds some data structure and may be loaded from remote	
	class Resource : public Unit
	{
	public:
		enum struct Status
		{
			NONE,
			SETUP,
			LOADED,
			NOT_FOUND,
			NO_AUTH,
			NO_ACCESS,
			CONFIG_ERR,
			DATA_INVALID
		};
		typedef std::function<void(ReqError)> ReadyHandler;

		Resource(std::string basename, std::string type_name) :
			m_buf(nullptr),
			m_buflen(0),
			m_status(Status::NONE),
			Unit(basename, type_name)
		{}

		// load the resource (may be asynchronus) - call handler on ready
		virtual void load(ReadyHandler handler) = 0;

		// get the status of the Unit
		Status getStatus() { return m_status; }

		// get raw data
		size_t getRaw(char** buf, size_t* buflen) { *buf = m_buf; *buflen = m_buflen; return m_buflen; }

		// check if resource finished loading
		bool isLoaded() { return m_status == Status::LOADED; }
	protected:
		Status m_status;
		char* m_buf;
		size_t m_buflen;
	};
}

#endif // !RHEA_RESOURCE_H
