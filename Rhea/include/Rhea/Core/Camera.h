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
		enum class CameraForwardAxis {
			PositiveZ,
			NegativeZ

		}; 
		Camera(std::string name, std::string type_name) :			
			Item(name, type_name)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		float getVerticalFovDegrees() const { return m_vertical_fov_degrees; }
		float getVerticalFOV() const { return m_vertical_fov_degrees * math::degToRad *0.5f; }
		CameraForwardAxis getForwardAxis() { return m_forward_axis; }

		float getNearClip() const { return m_near_clip; }
		float getFarClip() const { return m_far_clip; }
		bool hasFarClip() const { return m_has_far_clip; }

		float getAspectRatio() const { return m_aspect_ratio; }
		bool hasAspectRatio() const { return m_has_aspect_ratio; }

	private:
		float m_vertical_fov_degrees = 45.0f;
		enum class CameraForwardAxis m_forward_axis = CameraForwardAxis::PositiveZ;
		float m_near_clip = 0.1f;
		float m_far_clip = 1000.0f;
		bool m_has_far_clip = false;
		float m_aspect_ratio = 0.0f;
		bool m_has_aspect_ratio = false;
	};
}

#endif // !RHEA_CAMERA_H
