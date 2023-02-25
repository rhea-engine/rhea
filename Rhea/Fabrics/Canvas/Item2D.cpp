#include "Item2D.h"
#include "Item2D.h"
#include "Application.h"
#include "Fabrics/Media/ImageResource.h"
#include "TextResource.h"
#include "UnitTree.h"
#include "ResourceList.h"
#include <Psio/Log.h>
#include <Graphics/GraphicsService.h>

namespace Rhea
{
	void Item2D::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Item::setup(app, attributes, values);
		m_status = Status::SETUP;

		m_position.x = values["x"];
		m_position.y = values["y"];		
		m_size.x = values["w"];
		m_size.y = values["h"];

		m_resource_list = new ResourceList(app);

		if (m_values.find("texture") != m_values.end()) {
			m_resource_list->push("texture", m_values["texture"], "//meworla/texture");
			if (m_values.find("shader") == m_values.end()) {
				// set a default
				m_values["shader"] = "//rhea/shaders/widget.hlsl";
			}
			m_resource_list->push("shader", m_values["shader"], "//meworla/shader");
		}
	}

	Unit* Item2D::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Item2D(name, type);
		return ret;
	}

	void Item2D::ready()
	{
		if (m_status >= Status::READY)
			return;
		Unit::ready();

		// load and add textures
		if (m_values.find("texture") != m_values.end())
		{
			Unit* tex = (*m_resource_list)["texture"];
			if (tex) {

				// create dummy material
				m_material = new Material(m_path + "_material");
				m_material->setup(m_app);
				m_material->addTexture(static_cast<ImageResource*>(tex));
				m_material->setValue("shader", "widget");

				// get the shader
				Unit* shader_res = (*m_resource_list)["shader"];
				if (shader_res) {
					m_material->setShader(static_cast<TextResource*>(shader_res));
				}
				else {
					LOG(LOG_ERROR) << "item2d::ready(): Shader '" << (std::string)m_values["shader"] << "' for " << m_pathname << " not found";
				}

				// add the mesh
				if (!m_mesh) {

					std::unordered_map<std::string, std::string> attributes;
					std::unordered_map<std::string, Variant> values;
					m_mesh = static_cast<Mesh*>(m_app->getUnitTree()->createUnit(m_pathname + "_mesh", "//meworla/mesh", attributes, values));
					m_mesh->setMaterial(m_material);

					setMesh();
					
					m_render_component->setMesh(m_mesh);
				}
			}
			else {
				LOG(LOG_ERROR) << "item2d::ready(): texture '" << (std::string)m_values["texture"] << "' for " << m_pathname << " not found";
			}
		}
		m_status = Status::READY;
	}

	void Item2D::setMesh()
	{
		float z = 0.0f;
		float m_x = X();
		float m_y = Y();
		float m_w = W();
		float m_h = H();

		// adjust to height of viewport (y is negative)
		GraphicsService* gsrv = (GraphicsService*)m_app->getService("graphics");
		m_y = gsrv->getViewportHeight() - m_y;

		float m_u1 = 0.0f;
		float m_v1 = 0.0f;
		float m_u2 = 1.0f;
		float m_v2 = 1.0f;

		std::vector<Mesh::vertex> vertices =
		{
			{ m_x, m_y - m_h, z, m_u1, m_v2 }, // bottom-left
			{ m_x, m_y, z, m_u1, m_v1 }, // top-left
			{ m_x + m_w, m_y, z, m_u2, m_v1 }, // top-right
			{ m_x + m_w, m_y - m_h, z, m_u2, m_v2 }  // bottom-right
		};
		std::vector<unsigned long> v_indices = { 0, 1, 2, 0, 2, 3 };
		m_mesh->setBuffers(vertices, v_indices);
		m_mesh->sendEvent("update", nullptr);
	}
	
	void Rhea::Item2D::update()
	{
		if (m_mesh)
			setMesh();
		sendEvent("update", nullptr);
	}
}
