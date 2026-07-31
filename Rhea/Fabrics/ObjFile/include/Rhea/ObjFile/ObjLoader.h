/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_OBJLOADER_H
#define RHEA_OBJLOADER_H

#include <string>
#include "Rhea/Core/Geometry.h"
#include "Rhea/Core/Mesh.h"
#include <Math/Vector3.h>
#include <Math/Vector2.h>

namespace rhea
{
	class Application;

	class ObjLoader : public Geometry
	{
		MECLASS(ObjLoader, Geometry)
	public:	
		ObjLoader(std::string path_name, std::string type_name) :
			Geometry(path_name, type_name)
		{}		
		bool load(rhea::Mesh* mesh);

		std::vector<Unit::ResourceReference> getReferences();
	
		struct Vertex {
			math::Vector3 position;
			math::Vector3 normal;
			math::Vector3 color;
			math::Vector2 uv;
			math::Vector3 tangent;

			bool operator==(const Vertex& other) const {
				return position.equals(other.position)/* && normal.equals(other.normal)*/ && uv.equals(other.uv)/* && color.equals(other.color) */;
			}
		};		
	};
}

#endif // !RHEA_OBJLOADER_H
