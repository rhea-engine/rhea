/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/Camera.h"

namespace rhea
{
	void Camera::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item::setup(app, attributes, values);

		const auto fov = m_values.find("vertical_fov_degrees");
		if (fov != m_values.end())
			m_vertical_fov_degrees = static_cast<float>(fov->second);

		const auto forward_axis = m_values.find("forward_axis");
		if (forward_axis != m_values.end() && m_values["forward_axis"].toString()=="-z")
			m_forward_axis = CameraForwardAxis::NegativeZ;

		if (m_values.find("near_clip") != m_values.end())
			m_near_clip = static_cast<float>(m_values["near_clip"]);

		if (m_values.find("far_clip") != m_values.end()) {
			m_far_clip = static_cast<float>(m_values["far_clip"]);
			m_has_far_clip = true;
		}

		if (m_values.find("aspect_ratio") != m_values.end()) {
			m_aspect_ratio = static_cast<float>(m_values["aspect_ratio"]);
			m_has_aspect_ratio = true;
		}

		if (m_values.find("rotation") != m_values.end())
		{
			// already handled in Item
			return;
		}

		ensureTransform();

		if (m_values.find("lookAt") != m_values.end())
		{
			Variant look_at_sit = getValue("lookAt");
			if (look_at_sit != Variant() && look_at_sit.size() == 3) {
				float x = look_at_sit[0];
				float y = look_at_sit[1];
				float z = look_at_sit[2];
				setLookAt(math::Vector3(x, y, z), math::Vector3::UNIT_Z);
			}
		}
		else {
			// default look at
			setLookAt(math::Vector3(0.0f, 0.0f, 0.0f), math::Vector3::UNIT_Z);
		}
	}
}