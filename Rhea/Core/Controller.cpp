#include <Rhea/Core/Controller.h>

namespace rhea
{
	void Controller::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values)
	{
		Component::setup(app, attributes, values);

		if (m_values.find("default_character") != m_values.end()) {
			std::string ref = m_values["default_character"].toString();
			m_default_character.setId(ref);
		}
	}

	void Controller::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "default_character", &Controller::m_default_character, "//meworla/scene", ResourceUsage::Prepare);
	}
}