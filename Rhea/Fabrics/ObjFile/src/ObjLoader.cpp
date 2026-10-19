/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/ObjFile/ObjLoader.h"
#define TINYOBJLOADER_IMPLEMENTATION 
#include <tiny_obj_loader/tiny_obj_loader.h>

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Resources/TextResource.h>
#include <Psio/Log.h>
#include <functional>
#include <Rhea/Core/Shader.h>
#include <Rhea/Core/Material.h>
#include <Rhea/Resources/ImageResource.h>
#include <Rhea/Resources/VertexBufferStandard.h>
#include <Rhea/Resources/ResourceList.h>
#include <Rhea/Resources/ResourceService.h>

namespace rhea
{
	struct VertexKeyHasher {
		size_t operator()(const ObjLoader::Vertex& v) const {
			size_t hash = 17;
			hash = hash * 31 + std::hash<float>{}(v.position.x);
			hash = hash * 31 + std::hash<float>{}(v.position.y);
			hash = hash * 31 + std::hash<float>{}(v.position.z);
			//  hash = hash * 31 + std::hash<float>{}(v.normal.x);
			 // hash = hash * 31 + std::hash<float>{}(v.normal.y);
			//  hash = hash * 31 + std::hash<float>{}(v.normal.z);
			//  hash = hash * 31 + std::hash<float>{}(v.color.x);
			//  hash = hash * 31 + std::hash<float>{}(v.color.y);
			//  hash = hash * 31 + std::hash<float>{}(v.color.z);
			hash = hash * 31 + std::hash<float>{}(v.uv.x);
			hash = hash * 31 + std::hash<float>{}(v.uv.y);
			return hash;
		}
	};

	struct VertexKeyHasherUV
	{
		std::size_t operator()(const Mesh::vertex& v) const
		{
			using std::size_t;
			using std::hash;
			using std::string;

			size_t res = 17;
			res = res * 31 + hash<float>()(v.x);
			res = res * 31 + hash<float>()(v.y);
			res = res * 31 + hash<float>()(v.z);
			res = res * 31 + hash<float>()(v.u);
			res = res * 31 + hash<float>()(v.v);
			return res;
		}
	};

	struct ObjLoader::ParsedData {
		tinyobj::attrib_t attributes;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
	};

	ObjLoader::ObjLoader(std::string path_name, std::string type_name) :
		Geometry(path_name, type_name)
	{}

	void ObjLoader::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Geometry::setup(app, attributes, values);
		if (m_shader_ref.empty()) {
			m_shader_ref.setId(getValueString("shadername", "//rhea/shaders/standard"));
		}
	}

	void ObjLoader::prepare(PrepareHandler handler)
	{
		std::string objbuf;
		std::string mtlbuf;

		if (!m_mtl_ref.empty()) {
			TextResource* mtl = m_app->getUnitTree()->resolve(m_mtl_ref);
			if (mtl) {
				mtlbuf = mtl->getAsString();
			}
		}
		if (!m_obj_ref.empty()) {
			TextResource* obj = m_app->getUnitTree()->resolve(m_obj_ref);
			if (obj) {
				objbuf = obj->getAsString();
			}
			else {
				DLOG(LOG_DEBUG) << "Obj not found in objloader. Path was: " << m_obj_ref.id();
				handler(ReqError::RESOURCE_NOT_FOUND);
				return;
			}
		}
		else {
			LOG(LOG_ERROR) << "Could not open " << m_pathname << ": Obj file not defined.";
			handler(ReqError::RESOURCE_NOT_FOUND);
			return;
		}

		tinyobj::ObjReader reader;
		tinyobj::ObjReaderConfig config;
		bool change_handedness = true;

		bool ret = reader.ParseFromString(objbuf, mtlbuf, config);

		if (!ret) {
			LOG(LOG_ERROR) << "Could not parse obj file: " << reader.Error();
			handler(ReqError::DATA_INVALID);
			return;
		}

		m_parsed_data = std::make_unique<ParsedData>();
		m_parsed_data->attributes = reader.GetAttrib();
		m_parsed_data->shapes = reader.GetShapes();
		m_parsed_data->materials = reader.GetMaterials();


		ResourceContext texture_context;
		// root path for textures is the path of the mtl file
		texture_context.source_directory = m_mtl_ref.id().substr(0, m_mtl_ref.id().find_last_of('/'));

		ResourceList textures(m_app, texture_context);

		int i = 0;
		for (const auto& mat : m_parsed_data->materials) {
			if (!mat.diffuse_texname.empty()) {
				std::string resolved_texture_id = texture_context.normalizeResourceId(mat.diffuse_texname);
				m_resolved_diffuse_id.push_back(resolved_texture_id);
				textures.push(
					"diffuse_" + std::to_string(i),
					resolved_texture_id,
					"//meworla/texture");
			}
			else
				m_resolved_diffuse_id.push_back("");
			if (!mat.normal_texname.empty()) {
				std::string resolved_texture_id = texture_context.normalizeResourceId(mat.normal_texname);
				m_resolved_normal_id.push_back(resolved_texture_id);
				textures.push(
					"normal_" + std::to_string(i),
					resolved_texture_id,
					"//meworla/texture");
			}
			else
				m_resolved_normal_id.push_back("");
			i++;
		}

		auto* resource_service = m_app->getService<ResourceService>();
		resource_service->loadAsync(textures,
			[this, handler](ResourceList* loaded, Error* error) {
				if (error) {
					//handler(ReqError::DATA_INVALID);
					//return;
					LOG(LOG_ERROR) << "Error loading textures: " << error->getText();
				}

				// Store the loaded textures with their material assignment.
				m_prepared = true;
				handler(ReqError::OK);
			});
	}

	void ObjLoader::resolveReferences(const std::string& context_root, const std::string& source_root)
	{
		Geometry::resolveReferences(context_root, source_root);
	}

	bool ObjLoader::load(rhea::Mesh* mesh)
	{
		std::vector<VertexAttribute> layout;

		Shader* shader_res = m_app->getUnitTree()->resolve(m_shader_ref);
		if (!shader_res) {
			LOG(LOG_ERROR) << "Could not find shader resource: " << m_shader_ref.id();
			return false;
		}
		layout = shader_res->getVertexLayout();
		VertexBuffer* vertices = mesh->getVertexBuffer();

		bool change_handedness = true;
		bool ret = true;

		if (!m_parsed_data) {
			LOG(LOG_ERROR) << "ObjLoader::load: No parsed data available. Did you call prepare() first?";
			return false;
		}

		std::vector<Material*> unit_materials;

		// create the materials
		int ambient_id = shader_res->getInputSlotId("AMBIENT");
		int diffuse_id = shader_res->getInputSlotId("DIFFUSE");
		int specular_id = shader_res->getInputSlotId("SPECULAR");
		int emmission_id = shader_res->getInputSlotId("EMISSIVE");

		int basecolor_id = shader_res->getTextureSlotId("BASECOLOR");
		int normal_id = shader_res->getTextureSlotId("NORMAL");

		for (int slot = 0; slot < m_parsed_data->materials.size(); slot++) {
			Material* mat = new Material(m_parsed_data->materials[slot].name);
			// set shader
			mat->setShader(shader_res);
			mat->setValue("shader", getValue("shadername", "standard"));

			// set material values
			mat->setColor(ambient_id, math::Color(m_parsed_data->materials[slot].ambient[0], m_parsed_data->materials[slot].ambient[1], m_parsed_data->materials[slot].ambient[2], 1.0f));
			mat->setColor(diffuse_id, math::Color(m_parsed_data->materials[slot].diffuse[0], m_parsed_data->materials[slot].diffuse[1], m_parsed_data->materials[slot].diffuse[2], 1.0f));
			mat->setColor(specular_id, math::Color(m_parsed_data->materials[slot].specular[0], m_parsed_data->materials[slot].specular[1], m_parsed_data->materials[slot].specular[2], 1.0f));
			mat->setColor(emmission_id, math::Color(m_parsed_data->materials[slot].emission[0], m_parsed_data->materials[slot].emission[1], m_parsed_data->materials[slot].emission[2], 1.0f));

			int shining_id = shader_res->getInputSlotId("SHININESS");
			// set shininess - map for Phong [0..1000.0] -> [0...128.0]
			float shininess = m_parsed_data->materials[slot].shininess * 0.128f;
			mat->setFloat(shining_id, shininess);

			// set texture
			if (!m_parsed_data->materials[slot].diffuse_texname.empty()) {
				std::string resolved_texture_id = m_resolved_diffuse_id.size() > slot ? m_resolved_diffuse_id[slot] : "";
				ImageResource* diffuse_map = doCast<ImageResource>(m_app->getUnitTree()->find(resolved_texture_id));
				if (diffuse_map)
					mat->setTexture(basecolor_id, diffuse_map);
			}

			// set normal map
			if (normal_id != -1 && !m_parsed_data->materials[slot].normal_texname.empty()) {
				std::string resolved_texture_id = m_resolved_normal_id.size() > slot ? m_resolved_normal_id[slot] : "";
				ImageResource* normal_map = doCast<ImageResource>(m_app->getUnitTree()->find(resolved_texture_id));
				if (normal_map)
					mat->setTexture(normal_id, normal_map);
			}

			// finalize
			mesh->addChild(mat);
			if (mesh->getMaterial() == nullptr) {
				mesh->setMaterial(mat);
			}
			unit_materials.push_back(mat);
		}

		std::unordered_map<Vertex, unsigned long, VertexKeyHasher> unique_vertices;

		// get vertex sizes
		int vertex_size = 0;
		int vertex_elements = 0;
		for (auto& attribute : layout) {
			vertex_size += attribute.size;
			vertex_elements += attribute.element_count;
		}

		vertices->setVertexSize(vertex_size);
		size_t buf_size = 0;
		// add buffer for one vertex with vertex_elements floats
		float* raw_vertex = new float[vertex_elements];

		// prepare indices
		std::vector<std::vector<uint32_t>> indices(m_parsed_data->materials.size());

		for (const auto& shape : m_parsed_data->shapes) {
			// shape name
			DLOG(LOG_DEBUG) << "Shape loaded: " << shape.name;

			// indent offset
			size_t index_offset = 0;

			// reserve buffer for vertex in number of vertices (assume triangles)
			buf_size += shape.mesh.num_face_vertices.size() * 3;
			vertices->reserve(buf_size);

			// iterate over faces
			for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {

				// iterate over vertices composing face f
				int num_vertices = shape.mesh.num_face_vertices[f];
				for (size_t v = 0; v < num_vertices; v++) {

					Vertex vertex = {};

					// access to vertex
					tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

					// vertex coordinates
					vertex.position.x = m_parsed_data->attributes.vertices[3 * idx.vertex_index + 0];
					vertex.position.y = m_parsed_data->attributes.vertices[3 * idx.vertex_index + 1];
					vertex.position.z = m_parsed_data->attributes.vertices[3 * idx.vertex_index + 2];

					// right hand model ?
					if (change_handedness) {
						vertex.position.z *= -1.0f;
					}

					// vertex normal
					vertex.normal.x = m_parsed_data->attributes.normals[3 * idx.normal_index + 0];
					vertex.normal.y = m_parsed_data->attributes.normals[3 * idx.normal_index + 1];
					vertex.normal.z = m_parsed_data->attributes.normals[3 * idx.normal_index + 2];

					// vertex texture coordinates
					vertex.uv.x = m_parsed_data->attributes.texcoords[2 * idx.texcoord_index + 0];
					vertex.uv.y = m_parsed_data->attributes.texcoords[2 * idx.texcoord_index + 1];

					// flip v coordinate
					if (change_handedness)
						vertex.uv.y = 1.0f - vertex.uv.y;

					// vertex color
					vertex.color.x = m_parsed_data->attributes.colors[3 * idx.vertex_index + 0]; // red
					vertex.color.y = m_parsed_data->attributes.colors[3 * idx.vertex_index + 1]; // green
					vertex.color.z = m_parsed_data->attributes.colors[3 * idx.vertex_index + 2]; // blue

					if (unique_vertices.count(vertex) == 0)
					{
						// new vertex - store current position
						unique_vertices[vertex] = static_cast<unsigned long>(vertices->size());
						// iterate iver vertex attributes and add to vertex buffer
						size_t idx = 0;
						if (shader_res->getValue("input", "") == "standard") {
							VertexBufferStandard::VertexStandard v;
							v.position = vertex.position;
							v.normal = vertex.normal;
							v.texcoord = vertex.uv;
							v.tangent = { 0.0f, 0.0f, 0.0f };

							((VertexBufferStandard*)vertices)->push_back(v);

							/*


							for (auto& attribute : layout) {
								switch (attribute.type) {
								case VertexAttributeType::POSITION:
									raw_vertex[idx++] = vertex.position.x;
									raw_vertex[idx++] = vertex.position.y;
									raw_vertex[idx++] = vertex.position.z;
									break;
								case VertexAttributeType::NORMAL:
									raw_vertex[idx++] = vertex.normal.x;
									raw_vertex[idx++] = vertex.normal.y;
									raw_vertex[idx++] = vertex.normal.z;
									break;
								case VertexAttributeType::TEXCOORD:
									raw_vertex[idx++] = vertex.uv.x;
									raw_vertex[idx++] = vertex.uv.y;
									break;
								case VertexAttributeType::COLOR:
									raw_vertex[idx++] = vertex.color.x;
									raw_vertex[idx++] = vertex.color.y;
									raw_vertex[idx++] = vertex.color.z;
									break;
								}
							}

							vertices->push_back(raw_vertex);
							*/
						}
					}
					if (shape.mesh.material_ids[f] >= indices.size()) {
						LOG(LOG_ERROR) << "Material id " << shape.mesh.material_ids[f] << " is out of range for shape " << shape.name;
						return false;
					}
					indices[shape.mesh.material_ids[f]].push_back(unique_vertices[vertex]);
				}

				index_offset += num_vertices;
				// per-face material
				int face_id = shape.mesh.material_ids[f];
			}

			if (change_handedness) {
				for (size_t m = 0; m < m_parsed_data->materials.size(); m++)
					for (size_t i = 0; i < indices[m].size(); i += 3) {
						std::swap(indices[m][i + 1], indices[m][i + 2]);
					}
			}

			if (shader_res->getValue("input", "") == "standard") {

				// compute tangents
				for (size_t i = 0; i < m_parsed_data->materials.size(); i++)
				{
					// iterate over faces
					for (size_t face = 0; face < indices[i].size(); face += 3)
					{
						size_t idx1 = indices[i][face];
						size_t idx2 = indices[i][face + 1];
						size_t idx3 = indices[i][face + 2];

						VertexBufferStandard::VertexStandard* v1 = (*(VertexBufferStandard*)vertices)[idx1];
						VertexBufferStandard::VertexStandard* v2 = (*(VertexBufferStandard*)vertices)[idx2];
						VertexBufferStandard::VertexStandard* v3 = (*(VertexBufferStandard*)vertices)[idx3];

						math::Vector3 edge1 = v2->position - v1->position;
						math::Vector3 edge2 = v3->position - v1->position;
						math::Vector2 deltaUV1 = v2->texcoord - v1->texcoord;
						math::Vector2 deltaUV2 = v3->texcoord - v1->texcoord;

						float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

						math::Vector3 tangent;

						tangent = (edge1 * deltaUV2.y - edge2 * deltaUV1.y) * f;

						v1->tangent += tangent;
						v2->tangent += tangent;
						v3->tangent += tangent;
					}
				}

				for (unsigned int idx = 0; idx < vertices->size(); idx++) {
					VertexBufferStandard::VertexStandard* v = (*(VertexBufferStandard*)vertices)[idx];
					v->tangent.normalize();
				}
			}

			// store submeshes
			for (size_t i = 0; i < m_parsed_data->materials.size(); i++)
				mesh->setSubmesh(i, unit_materials[i], indices[i]);
		}
		// delete raw vertex
		delete[] raw_vertex;

		return ret;
	}

	void ObjLoader::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "path", &ObjLoader::m_obj_ref, "//meworla/wavefront-object", ResourceUsage::Load);
		registerResourceProperty(type, "material", &ObjLoader::m_mtl_ref, "//meworla/wavefront-material", ResourceUsage::Load);
	}
}
