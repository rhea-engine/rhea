#include "Rhea/Core/Camera.h"

namespace rhea
{
	void Camera::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item::setup(app, attributes, values);

		if (m_values.find("rotation") != m_values.end())
		{
			// already handled in Item
			return;
		}

		if (m_values.find("lookAt") != m_values.end())
		{
			Variant look_at_sit = getValue("lookAt");
			if (look_at_sit != Variant() && look_at_sit.size() == 3) {
				float x = look_at_sit[0];
				float y = look_at_sit[1];
				float z = look_at_sit[2];
				setLookAt(math::Vector3(x, y, z), math::Vector3::UNIT_Z);
			}
		}
		else {
			// default look at
			setLookAt(math::Vector3(0.0f, 0.0f, 0.0f), math::Vector3::UNIT_Z);
		}
	}
}