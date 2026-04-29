/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/SpotLight.h"

namespace rhea
{
	void SpotLight::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Light::setup(app, attributes, values);

		m_angle = getValue("angle", 0.0f);
	}
}