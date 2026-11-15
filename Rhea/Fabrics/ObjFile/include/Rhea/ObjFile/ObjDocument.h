/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_OBJDOCUMENT_H
#define RHEA_OBJDOCUMENT_H

#include <string>
#include "Rhea/Core/Mesh.h"
#include <Math/Vector3.h>
#include <Math/Vector2.h>
#include <Rhea/Resources/TextResource.h>
#include <Rhea/Resources/ResourceContext.h>

namespace rhea
{
	class Application;

	class ObjDocument : public Resource
	{
		RECLASS(ObjDocument, Resource, "//meworla/objdocument")
	public:	
		ObjDocument(std::string path_name, std::string type_name);
		~ObjDocument() override;
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		bool load(rhea::Mesh* mesh);

		void resolveReferences(const std::string& context_root, const std::string& source_root);
		static void initializeType(simpleparser::Type* type);
		void prepare(PrepareHandler handler) override;

		void prepareOwnData(PrepareHandler handler);

		void setSourceText(const std::string& text) {
			m_source_text = text;
			m_source_text_set = true;
		}

		const std::string& sourceText() const
		{
			return m_source_text;
		}

		void setResourceContext(const ResourceContext& context) {
			m_resource_context = context;
		}

		const ResourceContext& resourceContext() const {
			return m_resource_context;
		}

		const Ref<ObjDocument>& objectRef() const {
			return m_obj_ref;
		}
	
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

		bool extractTriangles(std::vector<math::Vector3>& positions, std::vector<uint32_t>& indices) const;

	protected:
		Ref<ObjDocument> m_obj_ref;
		Ref<TextResource> m_mtl_ref;
		Ref<Shader> m_shader_ref;
		std::string m_source_text;
		bool m_source_text_set = false;
		ResourceContext m_resource_context;
		TextResource* m_mtl_resource = nullptr;
		std::string m_mtl_source_text;

		void prepareMaterialLibraries(PrepareHandler handler);

	private:
		struct ParsedData;
		std::unique_ptr<ParsedData> m_parsed_data;

		std::vector<std::string> m_resolved_diffuse_id;
		std::vector<std::string> m_resolved_normal_id;
	};
}

#endif // !RHEA_OBJDOCUMENT_H
