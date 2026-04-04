#ifndef RHEA_RESOURCES_H
#define RHEA_RESOURCES_H

#include <string>

#include "../Unit.h"

namespace rhea
{
	class Resources : public Unit
	{
		MECLASS(Resources, Unit)
	public:
		Resources(std::string name, std::string type_name) :
			Unit(name, type_name)
		{}

		std::vector<Unit::Export> getReferences();
	};
}

#endif // !RHEA_RESOURCES_H
