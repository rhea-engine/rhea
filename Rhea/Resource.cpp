#include "Resource.h"
#include "ResourceList.h"
#include "Application.h"

namespace Rhea {
	
	Resource::~Resource()
	{
		if (m_resource_list)
			delete m_resource_list;
	}

	void Resource::load(Orion::ResourceRequest* req, ReadyHandler handler)
	{
		handler(ReqError::OK);
	}

	void Resource::prepare(Unit::ReadyHandler handler)
	{
		if (m_prepared) {
			handler(nullptr);
			return;
		}
		m_prepared = true;
		//m_app->log("prepare: " + m_path);

		// call parent asynchronously
		Unit::prepare([&](Error* err) {

			if (err) {
				m_app->log("Unit::prepare failed");
			}

			// do nothing if no resources are defined
			if (!m_resource_list) {
				handler(nullptr);
				return;
			}

			// load the resources
			m_resource_list->load([&](Error* err) {
				if (!err)
					m_status = Status::LOADED;
				else
					m_status = Status::DATA_INVALID;
				handler(err);
				});
			});
	}
	
	Unit* Resource::res(const std::string& key)
	{
		if (m_resource_list)
			return m_resource_list->get(key);
		return nullptr;
	}
}