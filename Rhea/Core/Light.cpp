#include "Rhea/Core/Light.h"

namespace rhea
{
	void Light::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item::setup(app, attributes, values);

		if (m_values.find("enabled") != m_values.end()) {
			m_is_enabled = m_values["enabled"];
		}

		// light properties for all lights

		Variant col_r = getValue("color");
		if (col_r.size() == 3) {
			m_color = Math::Vector3(col_r[0], col_r[1], col_r[2]);
		}
		else {
			m_color = Math::Vector3(1.0f, 1.0f, 1.0f);
		}
		Variant diffuse_r = getValue("diffuse");
		if (diffuse_r.size() == 4) {
			m_diffuse = Math::Quaternion(diffuse_r[0], diffuse_r[1], diffuse_r[2], diffuse_r[3]);
		}
		else {
			m_diffuse = Math::Quaternion(1.0f, 1.0f, 1.0f, 1.0f);
		}
		Variant specular_r = getValue("specular");
		if (specular_r.size() == 4) {
			m_specular = Math::Quaternion(specular_r[0], specular_r[1], specular_r[2], specular_r[3]);
		}
		else {
			m_specular = Math::Quaternion(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}	
}