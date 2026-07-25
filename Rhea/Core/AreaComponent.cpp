/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include <Rhea/Core/AreaComponent.h>
#include <Rhea/Application.h>
#include <Rhea/Core/AreaService.h>

namespace rhea
{
	void AreaComponent::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Component::setup(app, attributes, values);
/* TODO		if (values.find("enabled") != values.end()) {
			enabled = values["enabled"].toBool();
		}
		if (values.find("priority") != values.end()) {
			priority = values["priority"].toInt();
		}
		*/
		// register as component in parent
		if (m_parent && !m_parent->getComponent<AreaComponent>()) {
			m_parent->addComponent(this);
		}
	}

	void AreaComponent::onActivate()
	{
		auto* area_service = m_app->getService<AreaService>();
		if (area_service)
		{
			area_service->registerTarget(doCast<Item>(m_parent));
		}
	}

	void AreaComponent::onDeactivate()
	{
		auto* area_service = m_app->getService<AreaService>();
		if (area_service)
		{
			area_service->unregisterTarget(doCast<Item>(m_parent));
		}
	}
}