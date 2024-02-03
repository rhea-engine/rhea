#include "Skybox.h"
#include "Application.h"
#include "UnitTree.h"
#include <Rhea/Fabrics/Core/Material.h>
#include <Rhea/Fabrics/Core/Shader.h>
#include <Psio/Log.h>

namespace Rhea
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

		// skybox mesh
		if (!m_render_component) {
			m_render_component = new RenderComponent("renderer", "//meworla/rendercomponent");
			addChild(m_render_component);
		}
		// if no mesh defined, add a cube
		if (!m_render_component->getMesh()) {
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

			std::vector<unsigned long> cube_indices = {
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
			m_render_component->setMesh(cube);
		}
		
		// find camera
		if (m_values.find("camera") != m_values.end())
		{
			m_camera = doCast<Camera>(m_app->getUnitTree()->find(m_values["camera"], m_attributes["basepath"], false));
		}
		else {
			m_camera = m_app->getUnitTree()->findByType<Camera>();			
		}

		if (!m_render_component->getMesh()->getMaterial()) {
			Material* mat = doCast<Material>(m_app->getUnitTree()->find(m_values["shader"], m_attributes["basepath"], false));
			if (!mat) {
				mat = new Material(m_path + "_material", "//meworla/material");
				addChild(mat);
			}
			m_render_component->getMesh()->setMaterial(mat);

		}
	}

	void Rhea::Skybox::onFrame(float delta)
	{
		// set skybox to camera position
		if (!m_camera) {
			m_camera = m_app->getUnitTree()->findByType<Camera>();
		}
		if (m_camera) {
			Vector3 position = m_camera->getTransform()->get_derived_position()->get_worldspace();
			Vector3 old_pos = m_transform.get_derived_position()->get_worldspace();
			// check wheter we need to update the position
			if (position != old_pos) {
				m_transform.get_position()->setPosition(position);
				m_transform.update();				
			}
		}
	}
}