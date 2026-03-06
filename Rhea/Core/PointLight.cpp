#include "Rhea/Core/PointLight.h"

namespace Rhea
{
	void PointLight::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Light::setup(app, attributes, values);

		Variant attenuation = getValue("attenuation");
		if (attenuation.size() == 3)
		{
			m_attenuation = Math::Vector3(attenuation[0], attenuation[1], attenuation[2]);
		}
	}
}