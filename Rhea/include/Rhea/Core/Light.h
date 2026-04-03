#ifndef RHEA_LIGHT_H
#define RHEA_LIGHT_H

#include "Rhea/Core/Item.h"

namespace Rhea
{
	class Light : public Item
	{
		MECLASS(Light, Item)
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

		// get properties
		Math::Quaternion getAmbient() { return m_ambient; }
		Math::Quaternion getDiffuse() { return m_diffuse; }
		Math::Quaternion getSpecular() { return m_specular; }
		Math::Vector3 getColor() { return m_color; }

	protected:
		bool m_is_enabled;

		Math::Quaternion m_ambient;
		Math::Quaternion m_diffuse;
		Math::Quaternion m_specular;
		Math::Vector3 m_color;
	};
}
#endif // !RHEA_LIGHT_H
