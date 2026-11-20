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
#include <Math/Math.h>

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

		float getVerticalFovDegrees() const { return m_vertical_fov_degrees; }
		float getVerticalFOV() const { return m_vertical_fov_degrees * math::degToRad *0.5f; }

	private:
		float m_vertical_fov_degrees = 45.0f;
	};
}

#endif // !RHEA_CAMERA_H
