/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_ACTIVECOMPONENT_H
#define RHEA_ACTIVECOMPONENT_H

#include <Rhea/Component.h>

class ActiveComponent : public rhea::Component
{
	RECLASS(ActiveComponent, Component, "//meworla/activecomponent");
public:
	ActiveComponent(std::string name, std::string type_name) :
		rhea::Component(name, type_name)
	{}
	virtual void onFrame(float delta_time) {
		// Default implementation does nothing
	}
};


#endif // !RHEA_ACTIVECOMPONENT_H
