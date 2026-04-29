/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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
