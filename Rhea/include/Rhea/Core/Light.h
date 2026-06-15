/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_LIGHT_H
#define RHEA_LIGHT_H

#include "Rhea/Core/Item.h"

namespace rhea
{
	class Light : public Item
	{
		RECLASS(Light, Item, "//meworla/light");
		REPROPERTY(math::Quaternion, "ambient", Label("Ambient"), Edit, Read);
		REPROPERTY(math::Quaternion, "diffuse", Label("Diffuse"), Edit, Read);
		REPROPERTY(math::Quaternion, "specular", Label("Specular"), Edit, Read);
		REPROPERTY(math::Vector3, "color", Label("Color"), Edit, Read);
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
		void setAmbient(math::Quaternion ambient) { m_ambient = ambient; }
		math::Quaternion getAmbient() { return m_ambient; }
		void setDiffuse(math::Quaternion diffuse) { m_diffuse = diffuse; }
		math::Quaternion getDiffuse() { return m_diffuse; }
		void setSpecular(math::Quaternion specular) { m_specular = specular; }
		math::Quaternion getSpecular() { return m_specular; }
		void setColor(math::Vector3 color) { m_color = color; }
		math::Vector3 getColor() { return m_color; }

	protected:
		bool m_is_enabled;

		math::Quaternion m_ambient;
		math::Quaternion m_diffuse;
		math::Quaternion m_specular;
		math::Vector3 m_color;
	};
}
#endif // !RHEA_LIGHT_H
