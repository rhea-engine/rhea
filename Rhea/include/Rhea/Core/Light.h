/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_LIGHT_H
#define RHEA_LIGHT_H

#include "Rhea/Core/Item.h"
#include <Math/Color.h>

namespace rhea
{
	class Light : public Item
	{
		RECLASS(Light, Item, "//meworla/light");
		REPROPERTY(math::Color, "ambient", Label("Ambient"), Edit, Read);
		REPROPERTY(math::Color, "diffuse", Label("Diffuse"), Edit, Read);
		REPROPERTY(math::Color, "specular", Label("Specular"), Edit, Read);
		REPROPERTY(math::Color, "color", Label("Color"), Edit, Read);
	public:
		Light(std::string name, std::string type) :
			m_is_enabled(true),
			Item(name, type)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// return if light is enabled in scene
		bool isEnabled() const {
			return m_is_enabled;
		}
		// set if light is enabled in scene
		void setEnabled(bool enabled) {
			m_is_enabled = enabled;
		}

		// properties
		void setAmbient(math::Color ambient) { m_ambient = ambient; }
		math::Color getAmbient() { return m_ambient; }
		void setDiffuse(math::Color diffuse) { m_diffuse = diffuse; }
		math::Color getDiffuse() { return m_diffuse; }
		void setSpecular(math::Color specular) { m_specular = specular; }
		math::Color getSpecular() { return m_specular; }
		void setColor(math::Color color) { m_color = color; }
		math::Color getColor() { return m_color; }

	protected:
		bool m_is_enabled;

		math::Color m_ambient;
		math::Color m_diffuse;
		math::Color m_specular;
		math::Color m_color;
	};
}
#endif // !RHEA_LIGHT_H
