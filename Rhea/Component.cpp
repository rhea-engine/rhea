#include "Component.h"
#include <SimpleParser/Method.h>

namespace Rhea
{
	Component* Component::createType(std::string name, std::string type_name)
	{
		Component* ret = new Component(name, type_name);
		return ret;
	}

	void Component::initializeType(SimpleParser::Type* type)
	{
		type->registerMethod("setName", &Component::setName);
		type->registerMethod("getName", &Component::getPathName);

		type->registerProperty("path", SimpleParser::Type::get(SimpleParser::Type::BaseType::STRING), "setName", "getName");
	}
}