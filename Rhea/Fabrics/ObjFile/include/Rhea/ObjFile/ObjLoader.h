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
#include <Rhea/Resources/TextResource.h>

namespace rhea
{
	class Application;

	class ObjLoader : public Geometry
	{
		MECLASS(ObjLoader, Geometry)
	public:	
		ObjLoader(std::string path_name, std::string type_name);
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		bool load(rhea::Mesh* mesh);

		void resolveReferences(const std::string& context_root, const std::string& source_root);
		static void initializeType(simpleparser::Type* type);
		void prepare(PrepareHandler handler) override;
	
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
	protected:
		Ref<TextResource> m_obj_ref;
		Ref<TextResource> m_mtl_ref;
		Ref<Shader> m_shader_ref;

	private:
		struct ParsedData;
		std::unique_ptr<ParsedData> m_parsed_data;

		std::vector<std::string> m_resolved_diffuse_id;
		std::vector<std::string> m_resolved_normal_id;
	};
}

#endif // !RHEA_OBJLOADER_H
