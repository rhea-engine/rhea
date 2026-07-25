/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include <Rhea/Core/Area.h>
#include <Psio/Log.h>
#include <Rhea/Application.h>
#include <Rhea/Core/AreaService.h>

namespace rhea
{
	void Area::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item::setup(app, attributes, values);
	}

	void Area::ready()
	{
		Item::ready();

		// serach chidlren for a collider component
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			ColliderComponent* child = doCast<ColliderComponent>(it->second);
			if (child) {
				m_collider = child;
				break;
			}
		}

		if (!m_collider) {
			LOG(LOG_ERROR) << "Area " << getPathName() << " has no collider component.";
			return;
		}
		// only box colliders are supported for now
		if (!m_collider->shape || m_collider->shape->getShapeType() != "Box") {
			LOG(LOG_ERROR) << "Area " << getPathName() << " has a non-box collider component.";
			m_collider = nullptr;
			return;
		}
	}

	void Area::onActivate()
	{
		Item::onActivate();

		// register to area service
		auto* area_service = m_app->getService<AreaService>();
		if (area_service) {
			area_service->registerArea(this);
		}
	}

	void Area::onDeactivate()
	{
		Item::onDeactivate();

		// unregister from area service
		auto* area_service = m_app->getService<AreaService>();
		if (area_service) {
			area_service->unregisterArea(this);
		}
	}
}