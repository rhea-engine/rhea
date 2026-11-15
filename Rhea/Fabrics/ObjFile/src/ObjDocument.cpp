/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/ObjFile/ObjDocument.h"
#include <Rhea/ObjFile/ObjGeometry.h>
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
#include <sstream>
#include <algorithm>

namespace rhea
{
	struct VertexKeyHasher {
		size_t operator()(const ObjDocument::Vertex& v) const {
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

	struct ObjDocument::ParsedData {
		tinyobj::attrib_t attributes;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
	};

	ObjDocument::ObjDocument(std::string path_name, std::string type_name) :
		Resource(path_name, type_name)
	{}

	ObjDocument::~ObjDocument() = default;

	void ObjDocument::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Resource::setup(app, attributes, values);
		if (m_shader_ref.empty()) {
			m_shader_ref.setId(getValueString("shadername", "//rhea/shaders/standard"));
		}

		addComponentUnique<ObjGeometry>("geometry", "//meworla/objgeometry");
	}

	void ObjDocument::prepare(PrepareHandler handler)
	{
		if (m_source_text_set) {
			prepareMaterialLibraries(std::move(handler));
			return;
		}
		// use the referenced obj and mtl files to load the data
		if (!m_obj_ref.empty()) {
			ObjDocument* instance = m_app->getUnitTree()->resolve(m_obj_ref);
			if (instance) {
				//instance->prepare(std::move(handler));
				setSourceText(instance->sourceText());
				setResourceContext(instance->resourceContext());
				prepareMaterialLibraries(std::move(handler));
			}
			else {
				LOG(LOG_ERROR) << "Could not resolve obj reference: " << m_obj_ref.id();
				if (handler)
					handler(Error(Error::Code::NOT_FOUND, "Could not resolve obj reference", m_obj_ref.id()));
			}
		}
		else {
			LOG(LOG_ERROR) << "Could not open " << m_path << ": Obj file not defined.";
			if (handler)
				handler(Error(Error::Code::INVALID_DEFINITION, "Obj file not defined", m_path));
			return;
		}
	}

	std::vector<std::string> findMaterialLibraries(const std::string& obj_text)
	{
		std::vector<std::string> result;
		std::istringstream input(obj_text);
		std::string line;

		while (std::getline(input, line)) {
			std::istringstream fields(line);
			std::string keyword;
			fields >> keyword;

			if (keyword != "mtllib")
				continue;

			std::string filename;
			while (fields >> filename) {
				if (filename.front() == '#')
					break;

				if (std::find(result.begin(), result.end(), filename) == result.end()) {
					result.push_back(filename);
				}
			}
		}

		return result;
	}

	void ObjDocument::prepareMaterialLibraries(PrepareHandler handler)
	{
		if (!m_mtl_ref.empty()) {
			m_mtl_resource = m_app->getUnitTree()->resolve(m_mtl_ref);
			if (!m_mtl_resource) {
				LOG(LOG_ERROR) << "Could not resolve mtl reference: " << m_mtl_ref.id();
				if (handler)
					handler(Error(Error::Code::NOT_FOUND, "Could not resolve mtl reference", m_mtl_ref.id()));
				return;
			}
			m_mtl_source_text = m_mtl_resource->getAsString();
			prepareOwnData(handler);
		}
		else {
			// get mtllib from the OBJ text
			std::vector<std::string> mtl_paths = findMaterialLibraries(m_source_text).empty() ? std::vector<std::string>() : findMaterialLibraries(m_source_text);
			ResourceList mtl_resources(m_app, m_resource_context);
			int mat_index = 0;
			for (const auto& mtl_path : mtl_paths) {
				// resolve the path relative to the obj file
				auto resolved = m_resource_context.source_directory.empty() ? orion::ResourceAddress::parse(mtl_path) : orion::ResourceAddress::parse(m_resource_context.source_directory + mtl_path);
				if (!resolved.success) {
					LOG(LOG_ERROR) << "Could not resolve mtl file address: " << mtl_path;
					if (handler)
						handler(Error(Error::Code::INVALID_DATA, "Could not resolve mtl file address: " + mtl_path, m_obj_ref.id()));
					return;
				}
				const std::string resolved_mtl_id = resolved.address.toString();
				mtl_resources.push("mat_" + std::to_string(++mat_index), resolved_mtl_id, "//meworla/mtl");
			}
			auto* resource_service = m_app->getService<ResourceService>();

			resource_service->loadAsync(mtl_resources, [this, mat_index, handler](ResourceList* loaded, ErrorReport error) {
				if (error.isError()) {
					if (handler)
						handler(std::move(error));
					return;
				}

				m_mtl_source_text.clear();

				for (int index = 1; index <= mat_index; ++index) {
					const std::string key =	"mat_" + std::to_string(index);

					auto* mtl = doCast<TextResource>(loaded->get(key));
					if (!mtl) {
						if (handler) {
							handler(Error(Error::Code::INVALID_DATA, "Loaded material library is not a TextResource", key));
						}
						return;
					}

					m_mtl_source_text += mtl->getAsString();
					m_mtl_source_text += '\n';
				}

				prepareOwnData(handler);
				});
		}

	}

	void ObjDocument::prepareOwnData(PrepareHandler handler)
	{
		tinyobj::ObjReader reader;
		tinyobj::ObjReaderConfig config;
		bool change_handedness = true;
		m_resolved_diffuse_id.clear();
		m_resolved_normal_id.clear();

		bool ret = reader.ParseFromString(m_source_text, m_mtl_source_text, config);

		if (!ret) {
			LOG(LOG_ERROR) << "Could not parse obj file: " << reader.Error();
			if (handler)
				handler(Error(Error::Code::INVALID_DATA, "Could not parse obj file: " + reader.Error(), m_obj_ref.id()));
			return;
		}		

		m_parsed_data = std::make_unique<ParsedData>();
		m_parsed_data->attributes = reader.GetAttrib();
		m_parsed_data->shapes = reader.GetShapes();
		m_parsed_data->materials = reader.GetMaterials();

		auto texture_base = orion::ResourceAddress::parse(m_resource_context.source_directory);
		if (!texture_base.success) {
			LOG(LOG_ERROR) << "Could not get parent address of mtl file: " << reader.Error();
			if (handler)
				handler(Error(Error::Code::INVALID_DATA, "Could not get parent address of mtl file: " + reader.Error(), m_mtl_ref.id()));
			return;
		}
		ResourceContext texture_context;
		// root path for textures is the path of the mtl file
		texture_context.source_directory = texture_base.address.toString();

		ResourceList textures(m_app, texture_context);

		int i = 0;
		for (const auto& mat : m_parsed_data->materials) {
			if (!mat.diffuse_texname.empty()) {
				auto resolved = texture_base.address.resolveRelative(mat.diffuse_texname);
				if (!resolved.success) {
					LOG(LOG_ERROR) << "Could not resolve diffuse texture address: " << mat.diffuse_texname;
					if (handler)
						handler(Error(Error::Code::INVALID_DATA, "Could not resolve diffuse texture address: " + mat.diffuse_texname, m_mtl_ref.id()));
					return;
				}

				const std::string resolved_texture_id =	resolved.address.toString();
				m_resolved_diffuse_id.push_back(resolved_texture_id);
				textures.push("diffuse_" + std::to_string(i), resolved_texture_id, "//meworla/texture");
			}
			else
				m_resolved_diffuse_id.push_back("");
			if (!mat.normal_texname.empty()) {
				auto resolved = texture_base.address.resolveRelative(mat.normal_texname);
				if (!resolved.success) {
					LOG(LOG_ERROR) << "Could not resolve normal texture address: " << mat.normal_texname;
					if (handler)
						handler(Error(Error::Code::INVALID_DATA, "Could not resolve normal texture address: " + mat.normal_texname, m_mtl_ref.id()));
					return;
				}

				const std::string resolved_texture_id =	resolved.address.toString();
				m_resolved_normal_id.push_back(resolved_texture_id);
				textures.push("normal_" + std::to_string(i), resolved_texture_id, "//meworla/texture");
			}
			else
				m_resolved_normal_id.push_back("");
			i++;
		}

		auto* resource_service = m_app->getService<ResourceService>();
		resource_service->loadAsync(textures,
			[this, handler](ResourceList* loaded, ErrorReport error) {
				if (error.isError()) {
					for (auto err : error.errors) {
						LOG(LOG_ERROR) << "Error loading textures: " << err.getMessage();
					}
					if (handler)
						handler(error);
					return;
				}

				// Store the loaded textures with their material assignment.
				m_prepared = true;
				if (handler)
					handler(error);
			});
	}

	void ObjDocument::resolveReferences(const std::string& context_root, const std::string& source_root)
	{
		Resource::resolveReferences(context_root, source_root);
	}

	bool ObjDocument::load(rhea::Mesh* mesh)
	{
		if (!m_source_text_set) {
			auto* source = m_app->getUnitTree()->resolve(m_obj_ref);
			if (!source || source == this)
				return false;

			return source->load(mesh);
		}

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
			LOG(LOG_ERROR) << "ObjDocument::load: No parsed data available. Did you call prepare() first?";
			return false;
		}

		std::vector<Material*> unit_materials;

		// create the materials
		const int ambient_id = shader_res->getInputSlotId("AMBIENT");
		const int diffuse_id = shader_res->getInputSlotId("DIFFUSE");
		const int specular_id = shader_res->getInputSlotId("SPECULAR");
		const int emissive_id = shader_res->getInputSlotId("EMISSIVE");
		const int shininess_id = shader_res->getInputSlotId("SHININESS");

		const int basecolor_id = shader_res->getTextureSlotId("BASECOLOR");
		const int normal_id = shader_res->getTextureSlotId("NORMAL");

		for (int slot = 0; slot < m_parsed_data->materials.size(); slot++) {
			Material* mat = new Material(m_parsed_data->materials[slot].name);
			// set shader
			mat->setShader(shader_res);
			mat->setValue("shader", getValue("shadername", "standard"));

			// set material values
			mat->setColor(ambient_id, math::Color(m_parsed_data->materials[slot].ambient[0], m_parsed_data->materials[slot].ambient[1], m_parsed_data->materials[slot].ambient[2], 1.0f));
			mat->setColor(diffuse_id, math::Color(m_parsed_data->materials[slot].diffuse[0], m_parsed_data->materials[slot].diffuse[1], m_parsed_data->materials[slot].diffuse[2], 1.0f));
			mat->setColor(specular_id, math::Color(m_parsed_data->materials[slot].specular[0], m_parsed_data->materials[slot].specular[1], m_parsed_data->materials[slot].specular[2], 1.0f));
			mat->setColor(emissive_id, math::Color(m_parsed_data->materials[slot].emission[0], m_parsed_data->materials[slot].emission[1], m_parsed_data->materials[slot].emission[2], 1.0f));

			int shining_id = shader_res->getInputSlotId("SHININESS");
			// set shininess - map for Phong [0..1000.0] -> [0...128.0]
			float shininess = m_parsed_data->materials[slot].shininess * 0.128f;
			mat->setFloat(shining_id, shininess);

			// set texture
			if (!m_parsed_data->materials[slot].diffuse_texname.empty()) {
				std::string resolved_texture_id = m_resolved_diffuse_id.size() > slot ? m_resolved_diffuse_id[slot] : "";
				auto* diffuse_map = m_app->getUnitTree()->resolve(Ref<ImageResource>(resolved_texture_id));
				if (diffuse_map)
					mat->setTexture(basecolor_id, diffuse_map);
				else
					LOG(LOG_ERROR) << "OBJ diffuse texture not found: " << resolved_texture_id;
			}

			// set normal map
			if (normal_id != -1 && !m_parsed_data->materials[slot].normal_texname.empty()) {
				std::string resolved_texture_id = m_resolved_normal_id.size() > slot ? m_resolved_normal_id[slot] : "";
				auto* normal_map = m_app->getUnitTree()->resolve(Ref<ImageResource>(resolved_texture_id));
				if (normal_map)
					mat->setTexture(normal_id, normal_map);
				else
					LOG(LOG_ERROR) << "OBJ normal texture not found: " << resolved_texture_id;
			}

			// finalize
			mesh->addChild(mat);
			if (mesh->getMaterial() == nullptr) {
				mesh->setMaterial(mat);
			}
			unit_materials.push_back(mat);
		}

		const size_t fallback_slot = unit_materials.size();

		Material* fallback = new Material("fallback_material");
		// create a fallback material for meshes that have no material assigned
		fallback->setShader(shader_res);
		fallback->setValue("shader", shader_res->getPathName());

		if (diffuse_id >= 0)
			fallback->setColor(diffuse_id, math::Color(1, 1, 1, 1));
		if (ambient_id >= 0)
			fallback->setColor(ambient_id, math::Color(0, 0, 0, 1));
		if (specular_id >= 0)
			fallback->setColor(specular_id, math::Color(0, 0, 0, 1));
		if (emissive_id >= 0)
			fallback->setColor(emissive_id, math::Color(0, 0, 0, 1));
		if (shininess_id >= 0)
			fallback->setFloat(shininess_id, 1.0f);
		addChild(fallback);

		unit_materials.push_back(fallback);
		if (mesh->getMaterial() == nullptr) {
			mesh->setMaterial(fallback);
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
		std::vector<std::vector<uint32_t>> indices(unit_materials.size());		

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

				// prepare indices for each material
				const int material_id = shape.mesh.material_ids[f];

				size_t material_slot;
				if (material_id == -1) {
					material_slot = fallback_slot;
				}
				else if (material_id < 0 ||
					static_cast<size_t>(material_id) >= fallback_slot) {
					LOG(LOG_ERROR) << "Invalid material id " << material_id;
					return false;
				}
				else {
					material_slot = static_cast<size_t>(material_id);
				}

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
					if (material_slot >= indices.size()) {
						LOG(LOG_ERROR) << "Material id " << material_slot << " is out of range for shape " << shape.name;
						return false;
					}
					indices[material_slot].push_back(unique_vertices[vertex]);
				}

				index_offset += num_vertices;
				// per-face material
				int face_id = shape.mesh.material_ids[f];
			}

			if (change_handedness) {
				for (size_t m = 0; m < indices.size(); m++)
					for (size_t i = 0; i < indices[m].size(); i += 3) {
						std::swap(indices[m][i + 1], indices[m][i + 2]);
					}
			}

			if (shader_res->getValue("input", "") == "standard") {

				// compute tangents
				for (size_t i = 0; i < indices.size(); i++)
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

			size_t submesh_index = 0;

			// store submeshes
			for (size_t i = 0; i < indices.size(); ++i) {
				if (indices[i].empty())
					continue;
				mesh->setSubmesh(submesh_index++, unit_materials[i], indices[i]);
			}
		}
		// delete raw vertex
		delete[] raw_vertex;

		return ret;
	}

	bool ObjDocument::extractTriangles(std::vector<math::Vector3>& positions, std::vector<uint32_t>& indices) const 
	{
		positions.clear();
		indices.clear();

		// Explicit in a .scene defined wrapper document.
		if (!m_source_text_set) {
			auto* source = m_app->getUnitTree()->resolve(m_obj_ref);
			if (!source || source == this)
				return false;

			return source->extractTriangles(positions, indices);
		}

		// The document must already be prepared.
		if (!m_parsed_data)
			return false;

		const auto& attrib = m_parsed_data->attributes;

		positions.reserve(attrib.vertices.size() / 3);
		for (size_t i = 0; i + 2 < attrib.vertices.size(); i += 3) {
			positions.emplace_back(
				attrib.vertices[i],
				attrib.vertices[i + 1],
				-attrib.vertices[i + 2]); // same handedness as the mesh
		}

		for (const auto& shape : m_parsed_data->shapes) {
			size_t offset = 0;

			for (auto count : shape.mesh.num_face_vertices) {
				for (size_t k = 1; k + 1 < count; ++k) {
					const int a = shape.mesh.indices[offset].vertex_index;
					const int b = shape.mesh.indices[offset + k].vertex_index;
					const int c = shape.mesh.indices[offset + k + 1].vertex_index;

					if (a < 0 || b < 0 || c < 0 ||
						static_cast<size_t>(a) >= positions.size() ||
						static_cast<size_t>(b) >= positions.size() ||
						static_cast<size_t>(c) >= positions.size()) {
						positions.clear();
						indices.clear();
						return false;
					}

					// Reverse winding due to Z-mirroring.
					indices.push_back(static_cast<uint32_t>(a));
					indices.push_back(static_cast<uint32_t>(c));
					indices.push_back(static_cast<uint32_t>(b));
				}

				offset += count;
			}
		}

		return !positions.empty() && !indices.empty();
	}

	void ObjDocument::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "path", &ObjDocument::m_obj_ref, "//meworla/wavefront-object", ResourceUsage::Load);
		registerResourceProperty(type, "material", &ObjDocument::m_mtl_ref, "//meworla/wavefront-material", ResourceUsage::Load);
	}
}
