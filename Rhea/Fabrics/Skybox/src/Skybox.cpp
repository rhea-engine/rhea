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
	}

	std::vector<Unit::Export> Skybox::getReferences()
	{
		std::vector<Export> refs = Item::getReferences();		
		if (m_values.find("shader") != m_values.end()) {
			refs.push_back({ "shader", "//meworla/shader", m_values["shader"] });
		}
		return refs;
	}

	void Skybox::ready()
	{
		Item::ready();

		// make sure visualcomponent is set
		VisualComponent* visual_component = getComponent<VisualComponent>();
		if (!visual_component) {
			visual_component = addComponentUnique<VisualComponent>("visual");
		}
		// skybox mesh
		MeshComponent* mesh_component = getComponent<MeshComponent>();
		if (!mesh_component) {
			mesh_component = addComponentUnique<MeshComponent>("renderer");			
		}
		// if no mesh defined, add a cube
		if (!mesh_component->getMesh()) {
			// add a cube as child
			Mesh* cube = new Mesh(m_pathname + "_mesh");
			std::unordered_map<std::string, Variant> sub_values;
			sub_values["texture"] = m_values["texture"];
			sub_values["shader"] = "//rhea/shaders/skybox";
			sub_values["shadername"] = "skybox";
			cube->setup(m_app, m_attributes, sub_values);
			
			if (m_values.find("shader") != m_values.end()) {
				m_values["shader"] = "//rhea/shaders/skybox";
			}

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
		
		// find camera
		if (m_values.find("camera") != m_values.end())
		{
	//		m_camera = doCast<Camera>(m_app->getUnitTree()->find(m_values["camera"].toString(), m_attributes["basepath"], false));
		}
		else {
	//		m_camera = m_app->getUnitTree()->findByType<Camera>();			
		}

		if (!mesh_component->getMesh()->getMaterial()) {
			Material* mat = doCast<Material>(m_app->getUnitTree()->find(m_values["shader"].toString(), m_attributes["basepath"], false));
			if (!mat) {
				mat = new Material(m_path + "_material", "//meworla/material");
				addChild(mat);
			}
			mesh_component->getMesh()->setMaterial(mat);

		}
	}

	void Skybox::onActivate()
	{
		Item::onActivate();

		// find scene and scenes camera
		Scene* scene = findParentByType<Scene>();
		if (scene) {
			m_camera = scene->findByType<Camera>();
		}
		else {
			m_camera = m_app->getUnitTree()->findByType<Camera>();
		}
	}

	void Skybox::onDeactivate()
	{
		Item::onDeactivate();

		m_camera = nullptr;
	}

	void Skybox::onFrame(float delta)
	{
		// set skybox to camera position
		if (!m_camera) {
			return;
		}
		if (m_camera) {
			Vector3 position = m_camera->getTransform()->getWorldPosition().getTranslation();
			Vector3 old_pos = m_transform->getWorldPosition().getTranslation();
			// check wheter we need to update the position
			if (position != old_pos) {
				setWorldPosition(position);
			}
		}
	}
}