/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_RESOURCE_LIBRARY_H
#define RHEA_RESOURCE_LIBRARY_H

#include <string>

#include "../Unit.h"

namespace rhea
{
	class ResourceLibrary : public Unit
	{
		RECLASS(ResourceLibrary, Unit, "//meworla/resourcelibrary")
	public:
		ResourceLibrary(std::string name, std::string type_name) :
			Unit(name, type_name)
		{}

		std::vector<Unit::ResourceReference> getReferences();
	};
}

#endif // !RHEA_RESOURCE_LIBRARY_H
