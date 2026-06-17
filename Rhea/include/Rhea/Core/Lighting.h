/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_LIGHTING_H
#define RHEA_LIGHTING_H

#include <Math/Color.h>

#include <Rhea/Unit.h>

namespace rhea
{
	class Lighting : public Unit
	{
		RECLASS(Lighting, Unit, "//meworla/lighting");
		REPROPERTY(math::Color, "ambient", Label("Ambient"), Edit, Read);
	public:
		Lighting(std::string name, std::string type_name) :
			Unit(name, type_name)
		{}

		// setup
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// get ambient value
		math::Color getAmbient() { return m_ambient; }
		// set ambient value
		void setAmbient(math::Color ambient) { m_ambient = ambient; }

	protected:
		math::Color m_ambient;
	};
}
#endif // !RHEA_LIGHTING_H
