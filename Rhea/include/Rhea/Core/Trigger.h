/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_TRIGGER_H
#define RHEA_TRIGGER_H

#include <Rhea/Core/Area.h>
#include <Rhea/Components/ColliderComponent.h>

namespace rhea
{
	class Trigger : public Area
	{
		RECLASS(Trigger, Area, "//meworla/trigger");
	public:
		Trigger(std::string name, std::string type_name) :
			Area(name, type_name)
		{}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void ready();

		void onTargetEnter(Item* item) override;
		void onTargetExit(Item* item) override;
	};
}

#endif // !RHEA_TRIGGER_H