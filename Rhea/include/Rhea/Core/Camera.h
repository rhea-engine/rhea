/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_CAMERA_H
#define RHEA_CAMERA_H

#include "Rhea/Core/Item.h"

#include "Rhea/Core/Position.h"

namespace rhea
{
	class GraphicsCamera;

	class Camera : public Item
	{
		RECLASS(Camera, Item, "//meworla/camera")
	public:
		Camera(std::string name, std::string type_name) :			
			Item(name, type_name)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;
	};
}

#endif // !RHEA_CAMERA_H
