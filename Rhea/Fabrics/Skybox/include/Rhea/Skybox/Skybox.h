/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SKYBOX_H
#define RHEA_SKYBOX_H

#include "Rhea/Core/Item.h"
#include "Rhea/Core/Camera.h"

namespace rhea
{
	class Skybox : public Item
	{
		RECLASS(Skybox, Item, "//meworla/skybox")
	public:
		Skybox(std::string name, std::string type_name) :
			Item(name, type_name)
		{}
	
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		std::vector<Unit::ResourceReference> getReferences();

		void ready() override;	

		void onActivate() override;
		void onDeactivate() override;
	};
}
#endif // !RHEA_SKYBOX_H
