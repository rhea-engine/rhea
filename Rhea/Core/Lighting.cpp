#include "Rhea/Core/Lighting.h"

namespace rhea
{
	void Lighting::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		Variant ambient_r = getValue("ambient");
		if (ambient_r.size() == 4) {
			m_ambient = math::Quaternion(ambient_r[0], ambient_r[1], ambient_r[2], ambient_r[3]);
		}
	}
}
