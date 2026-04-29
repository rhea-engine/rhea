/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/DirectionalLight.h"

namespace rhea
{
	void DirectionalLight::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Light::setup(app, attributes, values);
	
		Variant dir_r = getValue("direction");
		if (dir_r.size() == 3) {
			m_direction = math::Vector3(dir_r[0], dir_r[1], dir_r[2]);
		}
	}
}