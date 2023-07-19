#ifndef RHEA_RESOURCE_H
#define RHEA_RESOURCE_H

#include <functional>
#include <vector>

#include "Orion/ResourceRequest.h"

#include "../Unit.h"

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
			READY,
			NOT_FOUND,
			NO_AUTH,
			NO_ACCESS,
			CONFIG_ERR,
			DATA_INVALID
		};
		typedef std::function<void(ReqError)> ReadyHandler;
		typedef std::function<void(ReqError, Unit*)> LoadedHandler;

		Resource(std::string basename, std::string type_name) :
			m_buf(nullptr),
			m_buflen(0),
			m_status(Status::NONE),
			m_prepared(false),
			Unit(basename, type_name)
		{}

		// destructor
		~Resource();

		// load the resource from request (may be asynchronus) - call handler on ready
		virtual void load(Orion::ResourceRequest* req, ReadyHandler handler);

		// get the status of the Unit
		Status getStatus() { return m_status; }

		// get raw data
		size_t getRaw(char** buf, size_t* buflen) { *buf = m_buf; *buflen = m_buflen; return m_buflen; }

		// set raw data
		void setRaw(size_t buflen, char* buf);

		// check if resource finished loading
		bool isLoaded() { return m_status == Status::LOADED; }

	protected:
		Status m_status;
		char* m_buf;
		size_t m_buflen;
		struct res_def {
			std::string id;
			std::string type;
		};
		bool m_prepared;
	};
}

#endif // !RHEA_RESOURCE_H
