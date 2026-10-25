/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_RESOURCE_H
#define RHEA_RESOURCE_H

#include <functional>
#include <vector>

#include <Orion/ResourceRequest.h>
#include <Orion/RequestError.h>

#include "../Unit.h"

using ReqError = orion::Error;

namespace rhea
{
	Error translateLoadError(orion::Error error, const std::string& resource_id);

	// a resource is a unit that holds some data structure and may be loaded from remote	
	class Resource : public Unit
	{
		MECLASS(Resource, Unit)
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
		typedef std::function<void(rhea::Error)> ReadyHandler;
		typedef std::function<void(rhea::Error, Unit*)> LoadedHandler;

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
		virtual void load(orion::ResourceRequest* req, ReadyHandler handler);

		// get the status of the Unit
		Status getStatus() { return m_status; }

		// get raw data
		size_t getRaw(char** buf, size_t* buflen) { *buf = m_buf; *buflen = m_buflen; return m_buflen; }

		// set raw data (moves the data to the resource)
		void setRaw(size_t buflen, char* buf);

		// check if resource finished loading
		bool isLoaded() { return m_status == Status::LOADED; }

		size_t getSize() { if (m_proxy_resource) return m_proxy_resource->getSize(); return m_buflen; }
		char* getData() { if (m_proxy_resource) return m_proxy_resource->getData(); return m_buf; }

		// instantiate
		struct InstanceRequest {
			std::string name;
			std::string basepath;
		};

		using InstanceHandler = std::function<void(Error, Unit*)>;
		virtual void buildInstance(const InstanceRequest& request, InstanceHandler handler);
		virtual void ensureReady(InstanceHandler handler);

		using PrepareHandler = std::function<void(Error)>;
		virtual void prepare(PrepareHandler handler);
		bool isPrepared() const { return m_prepared; }

	protected:
		Status m_status;
		char* m_buf;
		size_t m_buflen;
		struct res_def {
			std::string id;
			std::string type;
		};
		bool m_prepared;
		Resource* m_proxy_resource = nullptr; // if this resource is a proxy for another resource, this points to the real resource
	};
}

#endif // !RHEA_RESOURCE_H
