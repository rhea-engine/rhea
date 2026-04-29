/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/Avatar.h"

namespace rhea {
	
	void Avatar::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		Variant pos_sit = getValue("position");
		m_position.x = pos_sit[0];
		m_position.y = pos_sit[1];
		if (pos_sit.size() > 2)
			m_position.z = pos_sit[2];
		else
			m_position.z = 0.0f;
	}
}