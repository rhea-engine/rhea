/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/Mesh.h"
#include "Rhea/Application.h"
#include "Orion/ResourceStream.h"
#include "Rhea/UnitTree.h"
#include "Rhea/Resources/TextResource.h"
#include <Rhea/Core/Shader.h>
#include "Rhea/Resources/ImageResource.h"
#include "Rhea/Core/Material.h"
#include "Rhea/Resources/ResourceList.h"

#include <Psio/Log.h>
#include <Rhea/Core/Geometry.h>


namespace rhea
{
	Mesh::Mesh(const Mesh& m) :
		Unit(m),
		m_material(m.m_material),
		m_vertices(m.m_vertices),
		m_submeshes(m.m_submeshes),
		m_isDynamic(m.m_isDynamic),
		m_vertex(m.m_vertex),
		m_state(m.m_state),
		m_texture_ref(m.m_texture_ref),
		m_shader_ref(m.m_shader_ref),
		m_material_ref(m.m_material_ref),
		m_geometry_ref(m.m_geometry_ref)
	{
	}

	void Mesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		const auto value_it = m_values.find("diffuse_tex");
		if (value_it != m_values.end()) {
			const auto& textures = value_it->second;

			for (size_t i = 0; i < textures.size(); ++i)
				m_diffuse_texture_refs.emplace_back(textures[i].toString());
		}
		const auto normal_value_it = m_values.find("normal_tex");
		if (normal_value_it != m_values.end()) {
			const auto& textures = normal_value_it->second;

			for (size_t i = 0; i < textures.size(); ++i)
				m_normal_texture_refs.emplace_back(textures[i].toString());
		}
	}


	void Mesh::resolveReferences(const std::string& context_root, const std::string& source_root)
	{
		Unit::resolveReferences(context_root, source_root);

		const ResourceContext context{ context_root, source_root };

		for (auto& texture : m_diffuse_texture_refs)
			normalizeReference(texture, context);
		for (auto& texture : m_normal_texture_refs)
			normalizeReference(texture, context);
	}

	std::vector<Component::ResourceReference> Mesh::getReferences()
	{
		auto references = Unit::getReferences();

		for (size_t i = 0; i < m_diffuse_texture_refs.size(); ++i) {
			if (!m_diffuse_texture_refs[i].empty()) {
				references.push_back({
					"diffuse_tex_" + std::to_string(i),
					"//meworla/texture",
					m_diffuse_texture_refs[i].id()
					});
			}
		}

		for (size_t i = 0; i < m_normal_texture_refs.size(); ++i) {
			if (!m_normal_texture_refs[i].empty()) {
				references.push_back({
					"normal_tex_" + std::to_string(i),
					"//meworla/texture",
					m_normal_texture_refs[i].id()
					});
			}
		}

		return references;
	}

	void Mesh::instanceSetup(Component* source, Component* target, InstantiationContext& context)
	{
		Unit::instanceSetup(source, target, context);

		const auto* source_mesh = doCast<Mesh>(source);
		auto* target_mesh = doCast<Mesh>(target);

		target_mesh->m_diffuse_texture_refs = source_mesh->m_diffuse_texture_refs;
		target_mesh->m_normal_texture_refs = source_mesh->m_normal_texture_refs;
	}
	
	Material* Mesh::getDefaultMaterial()
	{
		if (m_material)
			return m_material;

		if (m_shader_ref.empty())
			m_shader_ref.setId("//rhea/shaders/standard");
		
		if (!m_app) {
			LOG(LOG_ERROR) << "Mesh: Application not set for mesh " << m_path << ". Did you call setup()?";
			return nullptr;
		}

		Material* material = new Material(m_path + "_material");
		addChild(material);
		material->setup(m_app, getAttributes(), { {"shader", m_shader_ref.id()}});
		Shader* shader_res = m_app->getUnitTree()->resolve(m_shader_ref);
		if (!shader_res) {
			LOG(LOG_ERROR) << "Mesh: Could not find shader " << m_shader_ref.id() << " for mesh " << m_path;
			material->destroy();
			return nullptr;
		}
		material->setShader(shader_res);
		int diffuse_id = shader_res->getInputSlotId("DIFFUSE");
		int emmission_id = shader_res->getInputSlotId("EMISSIVE");
		material->setColor(diffuse_id, math::Color(0.8f, 0.8f, 0.8f, 1.0f));
		material->setColor(emmission_id, math::Color(0.0f, 0.0f, 0.0f, 1.0f));

		m_material = material;
		return m_material;
	}

	void Mesh::ready()
	{
		if (!m_app) {
			LOG(LOG_ERROR) << "Mesh: Application not set for mesh " << m_path << ". Did you call setup()?";
			return;
		}

		if (m_state > 1) {
			DLOG(LOG_DEBUG) << "Mesh : Already ready, skipping " << m_path;
			return;
		}
		m_state = 1; // set to preparing

		Unit::ready();

		// load using geometry (normally used for import from file)
		if (!m_geometry_ref.empty()) {
			Geometry* geometry = m_app->getUnitTree()->resolve(m_geometry_ref);
			if (geometry) {
				geometry->load(this);
				m_state = 2;
			}
			else
				LOG(LOG_ERROR) << "Mesh: Could not find geometry " << m_geometry_ref.id() << " for mesh " << m_path;
		}

		// if material is set explicitly we use it
		if (!m_material_ref.empty()) {
			m_material = m_app->getUnitTree()->resolve(m_material_ref);
		}

		// override texture applies to all submeshes, if set
		ImageResource* tex = m_app->getUnitTree()->resolve(m_texture_ref);
				
		Material* override_material = nullptr;
		if (!m_material_ref.empty())
			override_material = m_app->getUnitTree()->resolve(m_material_ref);

		// set the override
		for (size_t i = 0; i < m_submeshes.size(); ++i) {
			Material* material = m_submeshes[i].material;

			bool material_copied = false;

			auto ensure_own_material = [&]()->bool {
				if (material_copied)
					return true;

				auto* copy = material->Clone();
				copy->setName("material_override_" + std::to_string(i));
				if (!addChild(copy, true)) {
					LOG(LOG_ERROR) << "Mesh "<< getPath() << ": Failed to add child material " << copy->getPath();
					copy->destroy();
					return false;
				}
				material = copy;
				material_copied = true;
				return true;
				};

			if (override_material)
				material = override_material;

			if (!material)
				material = getDefaultMaterial();

			// apply texture overrides.
			if (!m_diffuse_texture_refs.empty()) {
				ImageResource* diffuse_tex = m_app->getUnitTree()->resolve(m_diffuse_texture_refs[i % m_diffuse_texture_refs.size()]);
				if (diffuse_tex) {
					if (!ensure_own_material())
						continue;
					int diffuse_id = material->getShader()->getTextureSlotId("BASECOLOR");
					material->setTexture(diffuse_id, diffuse_tex);
				}
				else {
					LOG(LOG_WARNING) << "Mesh '" << getPath() << "': diffuse texture override not found: "
						<< m_diffuse_texture_refs[i % m_diffuse_texture_refs.size()].id()
						<< " (basePath=" << m_diffuse_texture_refs[i % m_diffuse_texture_refs.size()].basePath() << ")";
				}
			}
			else if (tex) {
				int basecolor_id = material->getShader()->getTextureSlotId("BASECOLOR");
				if (!ensure_own_material())
					continue;
				material->setTexture(basecolor_id, tex);
			}
			if (!m_normal_texture_refs.empty()) {
				ImageResource* normal_tex = m_app->getUnitTree()->resolve(m_normal_texture_refs[i % m_normal_texture_refs.size()]);
				if (normal_tex) {
					if (!ensure_own_material())
						continue;
					int normal_id = material->getShader()->getTextureSlotId("NORMAL");
					material->setTexture(normal_id, normal_tex);
				}
				else {
					LOG(LOG_WARNING) << "Mesh '" << getPath() << "': normal texture override not found: "
						<< m_normal_texture_refs[i % m_normal_texture_refs.size()].id()
						<< " (basePath=" << m_normal_texture_refs[i % m_normal_texture_refs.size()].basePath() << ")";
				}
			}

			m_submeshes[i].material = material;
		}

		m_state = 2; // set to ready
	}

	void Mesh::setMaterial(Material* material)
	{
		m_material = material;
		if (!m_submeshes.size())
			m_submeshes.resize(1);
		m_submeshes[0].material = material;
	}

	void Mesh::getBuffers(std::vector<vertex>& vertices, std::vector<uint32_t>& indices)
	{
		vertices = m_vertices;
		//		indices = rhea::Mesh::m_v_indices;
		if (m_submeshes.size())
			indices = m_submeshes[0].indices;
	}

	void Mesh::setBuffers(std::vector<vertex>& vertices, std::vector<uint32_t>& indices)
	{
		m_vertices = vertices;
		if (!m_submeshes.size()) {
			addSubmesh(m_material, indices);
		}
		else {
			m_submeshes[0].indices = indices;
			m_submeshes[0].material = m_material;
		}
	}

	void Mesh::setVertexBuffer(std::vector<VertexAttribute>& layout, std::unordered_map<std::string, VertexBuffer>& vertices, size_t num_vertices)
	{
		//		std::vector<VertexAttribute>& layout = m_material->getShader()->getVertexLayout();

		// get vertex sizes
		int vertex_size = 0;
		int vertex_elements = 0;
		for (auto& attribute : layout) {
			vertex_size += attribute.size;
			vertex_elements += attribute.element_count;
		}

		m_vertex.setVertexSize(vertex_size);
		size_t buf_size = 0;
		// add buffer for one vertex with vertex_elements floats
		float* raw_vertex = new float[vertex_elements];

		// iterate over vertices
		for (int j = 0; j < num_vertices; j++) {
			// iterate over attributes of input
			size_t idx = 0;
			for (auto& attribute : layout) {
				// store vertex in raw_vertex depending of attribute
				if (j < vertices[attribute.name].size()) {
					for (int i = 0; i < attribute.element_count; i++) {
						if (vertices[attribute.name][j])
							raw_vertex[(attribute.offset/sizeof(float)) + i] = vertices[attribute.name][j][i];
					}
				}
			}
			m_vertex.push_back(raw_vertex);			
		}
		delete[] raw_vertex;
	}

	void Mesh::setBuffers(std::unordered_map<std::string, VertexBuffer>& vertices, std::vector<uint32_t>& indices, size_t num_vertices)
	{
		if (!m_material) {
			LOG(LOG_ERROR) << "No main material assigned to mesh";
			return;
		}
		std::vector<VertexAttribute>& layout = m_material->getShader()->getVertexLayout();
		setVertexBuffer(layout, vertices, num_vertices);
		if (!m_submeshes.size())
			addSubmesh(m_material, indices);
		else
			m_submeshes[0].indices = indices;
	}

	void Mesh::addSubmesh(Material* material, std::vector<uint32_t>& indices)
	{
		//m_v_indices = indices;
		m_submeshes.push_back({indices, material});
	}

	void Mesh::setSubmesh(size_t idx, Material* material, std::vector<uint32_t>& indices)
	{
		if (idx >= m_submeshes.size())
			m_submeshes.resize(idx + 1);

		m_submeshes[idx] = { indices, material };
	}

	void Mesh::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "geometry", &Mesh::m_geometry_ref, "//meworla/geometryloader", ResourceUsage::Prepare);
		registerResourceProperty(type, "shader", &Mesh::m_shader_ref, "//meworla/shader", ResourceUsage::Load);
		registerResourceProperty(type, "texture", &Mesh::m_texture_ref, "//meworla/texture", ResourceUsage::Load);
		registerResourceProperty(type, "material", &Mesh::m_material_ref, "//meworla/material", ResourceUsage::Load);
	}
}
