#include "SpotLight.h"

namespace Rhea
{
	void SpotLight::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Light::setup(app, attributes, values);

		m_angle = getValue("angle", 0.0f);
	}
}