/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_ACTIVECOMPONENT_H
#define RHEA_ACTIVECOMPONENT_H

#include <Rhea/Component.h>

namespace rhea
{
	// ActiveComponent is a base class for components that need to perform actions every frame.
	// It provides an onFrame method that can be overridden by derived classes to implement custom behavior.
	class ActiveComponent : public Component
	{
		RECLASS(ActiveComponent, Component, "//meworla/activecomponent");
	public:
		ActiveComponent(std::string name, std::string type_name) :
			Component(name, type_name)
		{}
		virtual void onFrame(float delta_time) {
			// Default implementation does nothing
		}
	};
}

#endif // !RHEA_ACTIVECOMPONENT_H
