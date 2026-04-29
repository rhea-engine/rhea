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
		m_state(m.m_state)
	{
	}

	void Mesh::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		// dependent resources
		if (m_values.find("texture") != m_values.end() &&
			m_values.find("shader") == m_values.end()) {
			m_values["shader"] = "//rhea/shaders/standard";
		}
	}

	std::vector<Unit::Export> Mesh::getReferences()
	{
		std::vector<Export> refs;
		if (m_values.find("geometry") != m_values.end())
			refs.push_back({ "geometry", "//meworla/geometryloader", m_values["geometry"] });		
		if (m_values.find("shader") != m_values.end()) {
		//	refs.push_back({ "texture", "//meworla/texture", m_values["texture"] });
			refs.push_back({ "shader", "//meworla/shader", m_values["shader"] });
		}
		return refs;
	}

	void Mesh::ready()
	{
		if (m_state)
			return;
		m_state = 1;

		Unit::ready();
		
		// if we have a texture we create a correspondig material
		if (m_values.find("texture") != m_values.end())
		{
			Unit* tex = nullptr;

			// if material is set explicitly we use it
			if (m_values.find("material") != m_values.end()) {
				m_material = doCast<Material>(m_app->getUnitTree()->find(m_values["material"].toString(), m_attributes["basepath"], false));
			}

			tex = doCast<Unit>(m_app->getUnitTree()->find(m_values["texture"].toString(), m_attributes["basepath"], false));
			if (tex) {
				if (!m_material) {
					// if no material was set create one ad-hoc as child of this mesh
					m_material = new Material(m_path + "_material");
					addChild(m_material);
				}
				m_material->setup(m_app);
				m_material->setTexture(0, static_cast<ImageResource*>(tex));
				m_material->setValue("shader", getValue("shadername", "standard"));
				if (m_values.find("shadername") != m_values.end()) {
					LOG(LOG_DEBUG) << "shadername set: " << (std::string)m_values["shadername"];
				}
				if (m_submeshes.size() > 0) {
					m_submeshes[0].material = m_material;
				}

				// get the shader				
				Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(m_values["shader"].toString(), m_attributes["basepath"], false));
				if (shader_res) {
					m_material->setShaderRes(shader_res);
					int diffuse_id = shader_res->getInputSlotId("DIFFUSE");
					m_material->setColor(diffuse_id, math::Vector3(1.0f,1.0f,1.0f));
				    int basecolor_id = shader_res->getTextureSlotId("BASECOLOR");
					m_material->setTexture(basecolor_id, static_cast<ImageResource*>(tex));
				}
				else {
					LOG(LOG_ERROR) << "Could not find shader resource: " << (std::string)m_values["shader"];
				}
			}
		}	
		
		// load using geometry (normally used for import from file)
		if (m_values.find("geometry") != m_values.end()) {
			Geometry* geometry = doCast<Geometry>(m_app->getUnitTree()->find(m_values["geometry"].toString(), m_attributes["basepath"], false));
			if (geometry) {				
				geometry->load(this);
			}
		}
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
		//		std::vector<VertexAttribute>& layout = m_material->getShaderRes()->getVertexLayout();

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
		std::vector<VertexAttribute>& layout = m_material->getShaderRes()->getVertexLayout();
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
			m_submeshes.push_back({ indices, material });
		else
			m_submeshes[idx] = { indices, material };
	}
}