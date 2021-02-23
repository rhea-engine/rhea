#include <Poseidon/Core.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Entity.h>
#include <Poseidon/Mesh.h>
#include <Poseidon/RenderContext.h>

#include "Application.h"
#include "Unit.h"
#include "Material.h"

#include "GraphicsManager.h"
#include "MaterialManager.h"
#include "GraphicsService.h"
#include "RenderElement.h"

namespace Rhea
{
	Poseidon::Node* GraphicsManager::createRenderable(GraphicsRenderElement* elem, Poseidon::Core* core, Unit* item, std::string render_context_name)
	{
		Poseidon::SceneManager* scene_mgr = core->get_scene_manager();

		// get mesh if already defined
		Poseidon::Mesh* mesh = elem->getMesh();
		if (!mesh) {
			// create an empty mesh
			mesh = new Poseidon::Mesh();
			mesh->setViewport(scene_mgr
				->getViewport());
			elem->SetMesh(mesh);
		}
		// create an entity
		Poseidon::Node* entity = core->createEntity(mesh);	
		elem->SetRenderEntity(entity);		
		
		Poseidon::RenderContext* render_context = scene_mgr->getViewport()->getRenderContext(render_context_name);
		if (!render_context) {
			item->getApplication()->log("GraphicsText::createText: render context '" + render_context_name + "' not found");
			return nullptr;
		}
		render_context->addRenderChild(static_cast<Poseidon::Entity*>(entity));

		return entity;
	}

	void GraphicsManager::addPosition(Unit* item, Poseidon::Node* entity)
	{
		Poseidon::Position* pos = entity->get_position();
		
		Variant pos_sit = item->getValue("position");
		if (pos_sit != Variant()) {
			float x = pos_sit[0];
			float y = pos_sit[1];	
			float z = 0.0f;
			if (pos_sit.size() > 2) {
				z = pos_sit[2];
			}
			
			pos->setPosition(x, y, z);
		}
		else
			pos->setPosition(0.0f, 0.0f, 0.0f);
	}

	void GraphicsManager::setMaterial(Poseidon::Core* core, Material* material, Poseidon::Node* entity)
	{
		// get the material element
		GraphicsMaterial* mat_elem = static_cast<GraphicsMaterial*>(material->getElement("graphics"));
		if (!mat_elem) {
			// create it in material manager
			std::string mat_name = material->getPathName();
			GraphicsMaterialManager* mat_mgr = static_cast<GraphicsMaterialManager*>(m_service->getManager("//meworla/material"));
			mat_elem = static_cast<GraphicsMaterial*>(mat_mgr->createElement(core, material));
		}

		static_cast<Poseidon::Entity*>(entity)->setMaterial(mat_elem->m_material);
	}
}