/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/Item2D.h"
#include "Rhea/Application.h"
#include "Rhea/Resources/ImageResource.h"
#include "Rhea/UnitTree.h"
#include "Rhea/Resources/ResourceList.h"
#include <Psio/Log.h>
#include <Rhea/Graphics/IGraphicsService.h>
#include <Rhea/Core/Shader.h>
#include <Rhea/Components/MeshComponent.h>
#include <Rhea/Core/Mesh.h>
#include <Rhea/Graphics/SizeEvent.h>

namespace rhea
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

	std::vector<Unit::ResourceReference> Item2D::getReferences()
	{
		std::vector<ResourceReference> refs = {};//Item::getReferences();
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

		registerEventHandler("size", this, [this](Event* ev, Unit* sender) { return onViewportSize(ev, sender); });

		if (m_bg_color.a > 0.0f || m_bg_image || m_values.find("texture") != m_values.end()) {
			// background color or image was set

			if (m_values.find("texture") != m_values.end()) {
				ImageResource* tex = doCast<ImageResource>(m_app->getUnitTree()->find(m_values["texture"].toString(), m_attributes["basepath"], false));
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

	bool Item2D::onViewportSize(Event* ev, Unit* sender)
	{
		if (ev && ev->isEvent("size")) {
			SizeEvent* size_ev = static_cast<SizeEvent*>(ev);
			int width = size_ev->width;
			int height = size_ev->height;
			if (width != m_viewport_width || height != m_viewport_height) {
				m_viewport_width = width;
				m_viewport_height = height;
				m_render_dirty = true;
			}
		}
		return false;
	}

	void Item2D::computeMesh()
	{
		float z = 0.0f;
		float m_x = X();
		float m_y = Y();
		float m_w = W();
		float m_h = H();

		// adjust to height of viewport (y is negative)		
		m_y = m_viewport_height - m_y;

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
		std::vector<uint32_t> v_indices = { 0, 1, 2, 0, 2, 3 };
		MeshComponent* mesh_component = getComponent<MeshComponent>();
		if (mesh_component && mesh_component->getMesh()) {
			mesh_component->getMesh()->setBuffers(vertices, v_indices);
			mesh_component->getMesh()->sendEvent("update", nullptr);
		}
	}
	
	void rhea::Item2D::setBackgroundColor(int r, int g, int b, float opacity)
	{
		math::Color bg_color;
		bg_color.r = r / 255.0f;
		bg_color.g = g / 255.0f;
		bg_color.b = b / 255.0f;
		bg_color.a = opacity;
		if (bg_color != m_bg_color) {
			m_bg_color = bg_color;
			m_material_dirty = true;
			invalidateRender();
		}
	}

	void Item2D::setBackgroundImage(ImageResource* tex)
	{
		if (tex != m_bg_image) {
			m_bg_image = tex;
			m_material_dirty = true;
			invalidateRender();
		}
	}

	void Item2D::setBackgroundImage(Ref<ImageResource> tex_ref)
	{
		m_bg_image_ref = tex_ref;
		ImageResource* tex = tex_ref.resolve(m_app->getUnitTree());
		setBackgroundImage(tex);
	}

	void Item2D::syncRender()
	{
		if (m_material_dirty) {
			UpdateMaterial();
			m_material_dirty = false;
			m_needs_update = true;
		}

		if (m_render_dirty) {
			computeMesh();
			m_render_dirty = false;
			m_needs_update = true;
		}

		if (m_needs_update) {
			sendEvent("update", nullptr);
			m_needs_update = false;
		}
	}

	void rhea::Item2D::UpdateMaterial()
	{
		// only update if really changed
		if (!m_material_dirty)
			return;

		//LOG(LOG_INFO) << "Item2D::UpdateMaterial() for '" << m_pathname << "', is dirty: " << m_material_dirty;

		// (re-)create material
		//if (!m_material)
		m_material = nullptr; // old material gets destroyed 
		m_material = std::make_unique<Material>(m_path + "_material");
		m_material->setup(m_app, {}, {});
		m_material->setValue("shader", "widget");

		// get the shader
		Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(m_values["shader"].toString(), m_attributes["basepath"], false));
		if (shader_res) {
			m_material->setShader(shader_res);
		}
		else {
			LOG(LOG_ERROR) << "item2d::ready(): Shader '" << (std::string)m_values["shader"] << "' for " << m_pathname << " not found";
			return;
		}

		// make sure visualcomponent is set
		VisualComponent* visual_component = getComponent<VisualComponent>();
		if (!visual_component) {
			visual_component = addComponentUnique<VisualComponent>("visual_component");
		}
		// make sure rendercomponent is set
		MeshComponent* mesh_component = getComponent<MeshComponent>();
		if (!mesh_component) {
			mesh_component = addComponentUnique<MeshComponent>("mesh_component");
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
		if (!mesh_component->getMesh()) {

			std::unordered_map<std::string, std::string> attributes;
			std::unordered_map<std::string, Variant> values;
			Mesh* mesh = m_app->getUnitTree()->createUnit<Mesh>(m_pathname + "_mesh", attributes, values);
			mesh->setMaterial(m_material.get());
			addChild(mesh);
			mesh_component->setMesh(mesh);
		}
		else {
			// set current material
			Mesh* mesh = mesh_component->getMesh();
			mesh->setMaterial(m_material.get());
		}
	}

	void Item2D::onParented()
	{
		Item::onParented();
		if (!m_viewport_width || !m_viewport_height) {
			Item2D* p = doCast<Item2D>(m_parent);
			if (!p) {
				return;
			}
			int width = p->m_viewport_width;
			int height = p->m_viewport_height;
			setViewportSize(width, height);
		}
	}

	void Item2D::syncRenderTree()
	{
		syncRender();
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			Item2D* child = doCast<Item2D>(it->second);
			if (child) {
				child->syncRenderTree();
			}
		}
	}

	void Item2D::setViewportSize(int width, int height)
	{
		if (width != m_viewport_width || height != m_viewport_height) {			
			m_viewport_width = width;
			m_viewport_height = height;
			
			// need to update render data for this item
			m_render_dirty = true;

			// iterate to children and update their viewport size
			for (auto it = m_children.begin(); it != m_children.end(); ++it) {
				Item2D* child = doCast<Item2D>(it->second);
				if (child) {
					child->setViewportSize(m_viewport_width, m_viewport_height);
				}
			}
		}
	}
}
