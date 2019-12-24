#include <string>

#include "Object.h"

namespace Rhea
{
	bool Type::is_a(char* type_name)
	{
		return !strcmp(type_name, m_type_name);
	}

	void Type::setTypeName(char* type_name)
	{
		m_type_name = type_name;
	}
}