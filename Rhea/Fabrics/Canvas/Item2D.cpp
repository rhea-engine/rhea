#include "Item2D.h"
#include "Application.h"
#include "Fabrics/Media/ImageResource.h"
#include "UnitTree.h"
#include "Resources/ResourceList.h"
#include <Psio/Log.h>
#include <Graphics/GraphicsService.h>
#include <Rhea/Core/Shader.h>

namespace Rhea
{
	void Item2D::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		// rendering options
		if (m_values.find("stencil_test") == m_values.end())
			m_values["stencil_test"] = "disabled";

		m_position.x = values["x"];
		m_position.y = values["y"];
		m_size.x = values["w"];
		m_size.y = values["h"];

		if (m_values.find("shader") == m_values.end()) {
			// set a default
			m_values["shader"] = "//rhea/shaders/widget";
		}
	}

	std::vector<Unit::Export> Item2D::getReferences()
	{
		std::vector<Export> refs = {};//Item::getReferences();
		if (m_values.find("texture") != m_values.end()) {
			refs.push_back({ "texture", "//meworla/texture", m_values["texture"] });
		}
		if (m_values.find("shader") != m_values.end()) {
			refs.push_back({ "shader", "//meworla/shader", m_values["shader"] });
		}
		return refs;
	}
	
	void Item2D::ready()
	{
		Unit::ready();

		if (m_bg_color.w > 0.0f || m_values.find("texture") != m_values.end()) {
			// background color or image was set

			if (m_values.find("texture") != m_values.end()) {
				ImageResource* tex = doCast<ImageResource>(m_app->getUnitTree()->find(m_values["texture"], m_attributes["basepath"], false));
				if (tex) {
					// make sure texture is ready
					tex->ready();
					// add to material
					m_bg_image = tex;
				}
				else {
					LOG(LOG_ERROR) << "texture for item2d set but not found: " << (std::string)m_values["texture"];
				}
			}
			m_material_dirty = true;
			UpdateMaterial();
		}
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
		RenderComponent* render_component = getComponent<RenderComponent>();
		if (render_component && render_component->getMesh()) {
			render_component->getMesh()->setBuffers(vertices, v_indices);
			render_component->getMesh()->sendEvent("update", nullptr);
		}
	}
	
	void Rhea::Item2D::update()
	{
		if (getComponent<RenderComponent>() && getComponent<RenderComponent>()->getMesh())
			setMesh();
		sendEvent("update", nullptr);
	}

	void Rhea::Item2D::setBackgroundColor(int r, int g, int b, float opacity)
	{
		Math::Vector4 bg_color;
		bg_color.x = r / 255.0f;
		bg_color.y = g / 255.0f;
		bg_color.z = b / 255.0f;
		bg_color.w = opacity;
		if (bg_color != m_bg_color) {
			m_bg_color = bg_color;
			m_material_dirty = true;
		}
	}

	void Item2D::setBackgroundImage(ImageResource* tex)
	{
		if (tex != m_bg_image) {
			m_bg_image = tex;
			m_material_dirty = true;
		}
	}

	void Rhea::Item2D::UpdateMaterial()
	{
		// only update if really changed
		if (!m_material_dirty)
			return;
		else {
			m_material_dirty = false;

			// (re-)create material
			//if (!m_material)
			m_material = nullptr; // old material gets destroyed 
			m_material = std::make_unique<Material>(m_path + "_material");
			m_material->setup(m_app);
			m_material->setValue("shader", "widget");

			// get the shader
			Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(m_values["shader"], m_attributes["basepath"], false));
			if (shader_res) {
				m_material->setShaderRes(shader_res);
			}
			else {
				LOG(LOG_ERROR) << "item2d::ready(): Shader '" << (std::string)m_values["shader"] << "' for " << m_pathname << " not found";
				return;
			}
		}

		// make sure rendercomponent is set
		RenderComponent* render_component = getComponent<RenderComponent>();
		if (!render_component) {
			render_component = addComponentUnique<RenderComponent>("renderer");
		}

		// set or clear texture
		m_material->setTexture(0, m_bg_image);

		// set bg colors
		m_material->setColor(0, m_bg_color);

		// set tiling
		m_material->setFloat2VS(0, m_tiling);
		// set scaling
		m_material->setFloat2VS(1, m_scaling);

		// add the mesh if not set
		if (!render_component->getMesh()) {

			std::unordered_map<std::string, std::string> attributes;
			std::unordered_map<std::string, Variant> values;
			Mesh* mesh = m_app->getUnitTree()->createUnit<Mesh>(m_pathname + "_mesh", attributes, values);
			mesh->setMaterial(m_material.get());
			addChild(mesh);
			render_component->setMesh(mesh);

			setMesh();
		}
		else {
			// set current material
			Mesh* mesh = render_component->getMesh();
			mesh->setMaterial(m_material.get());
		}
	}
}
