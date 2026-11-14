/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GEOMETRY_H
#define RHEA_GEOMETRY_H

#include <Rhea/Unit.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Component.h>

namespace rhea
{
	class Geometry : public Component
	{
		MECLASS_VIRTUAL(Geometry, Component)
	public:
		Geometry(std::string path_name, std::string type_name) :
			Component(path_name, type_name)
		{}		
		virtual bool load(Mesh* mesh) = 0;
	};
}

#endif // !RHEA_GEOMETRY_H
