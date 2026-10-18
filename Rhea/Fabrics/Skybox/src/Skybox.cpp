/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Skybox/Skybox.h"
#include "Rhea/Application.h"
#include "Rhea/UnitTree.h"
#include <Rhea/Core/Material.h>
#include <Rhea/Core/Shader.h>
#include <Psio/Log.h>
#include <Rhea/Components/MeshComponent.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Core/Scene.h>

namespace rhea
{
	void Skybox::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item::setup(app, attributes, values);
		if (m_shader_resource.empty())
			m_shader_resource.setId("//rhea/shaders/skybox");
	}

	void Skybox::ready()
	{
		Item::ready();

		// make sure visualcomponent is set
		VisualComponent* visual_component = getComponent<VisualComponent>();
		if (!visual_component) {
			visual_component = addComponentUnique<VisualComponent>("visual");
		}

		// set to background phase
		visual_component->setRenderPhase(RenderPhase::Background);

		// skybox mesh
		MeshComponent* mesh_component = getComponent<MeshComponent>();
		if (!mesh_component) {
			mesh_component = addComponentUnique<MeshComponent>("mesh");			
		}
		// if no mesh defined, add a cube
		if (!mesh_component->getMesh()) {
			// add a cube as child
			Mesh* cube = new Mesh(m_pathname + "_mesh");
			cube->setTextureRef(m_texture_resource);			
			cube->setShaderRef(m_shader_resource);
			std::unordered_map<std::string, Variant> sub_values;
			sub_values["shadername"] = "skybox";
			cube->setup(m_app, m_attributes, sub_values);					

			// set the cube vertices

			std::vector<Mesh::vertex> cube_vertices = {
				{ -100.0f,  100.0f, -100.0f, 0.0f, 0.0f }, // Front top-left
				{  100.0f,  100.0f, -100.0f, 100.0f, 0.0f }, // Front top-right
				{  100.0f, -100.0f, -100.0f, 100.0f, 100.0f }, // Front bottom-right
				{ -100.0f, -100.0f, -100.0f, 0.0f, 100.0f }, // Front bottom-left

				{ -100.0f,  100.0f,  100.0f, 0.0f, 0.0f }, // Back top-left
				{  100.0f,  100.0f,  100.0f, 100.0f, 0.0f }, // Back top-right
				{  100.0f, -100.0f,  100.0f, 100.0f, 100.0f }, // Back bottom-right
				{ -100.0f, -100.0f,  100.0f, 0.0f, 100.0f }  // Back bottom-left
			};

			std::vector<uint32_t> cube_indices = {
				0, 1, 2, // Front face
				0, 2, 3, // Front face
				4, 5, 6, // Back face
				4, 6, 7, // Back face
				0, 1, 5, // Top face
				0, 5, 4, // Top face
				2, 3, 7, // Bottom face
				2, 7, 6, // Bottom face
				0, 3, 7, // Left face
				0, 7, 4, // Left face
				1, 2, 6, // Right face
				1, 6, 5  // Right face
			};

			std::vector<Mesh::vertex> cube_vertices2 = cube_vertices;

			cube->setBuffers(cube_vertices2, cube_indices);

			addChild(cube);
			cube->ready();
			mesh_component->setMesh(cube);
		}

		if (!mesh_component->getMesh()->getMaterial()) {
			Material* mat = m_app->getUnitTree()->resolve(m_material_resource);
			if (!mat) {
				mat = new Material(m_path + "_material", "//meworla/material");
				addChild(mat);
			}
			mesh_component->getMesh()->setMaterial(mat);

		}
	}

	void Skybox::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "texture", &Skybox::m_texture_resource, "//meworla/texture", ResourceUsage::Load);
		registerResourceProperty(type, "shader", &Skybox::m_shader_resource, "//meworla/shader", ResourceUsage::Load);
		registerResourceProperty(type, "material", &Skybox::m_material_resource, "//meworla/material", ResourceUsage::Load);
	}
}