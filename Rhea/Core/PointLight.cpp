/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/PointLight.h"

namespace rhea
{
	void PointLight::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Light::setup(app, attributes, values);

		Variant attenuation = getValue("attenuation");
		if (attenuation.size() == 3)
		{
			m_attenuation = math::Vector3(attenuation[0], attenuation[1], attenuation[2]);
		}
	}
}