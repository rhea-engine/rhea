#include <Rhea/Core/Controller.h>

namespace rhea
{
	void Controller::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "default_character", &Controller::m_default_character, "//meworla/scene", ResourceUsage::Prepare);
	}
}