#include <Poseidon/Core.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Entity.h>
#include <Poseidon/Mesh.h>
#include <Poseidon/RenderContext.h>

#include "Application.h"
#include "Unit.h"
#include "Core/Material.h"

#include "GraphicsManager.h"
#include "MaterialManager.h"
#include "GraphicsService.h"
#include "RenderElement.h"
#include <Psio/Log.h>
#include "../Fabrics/GUI/include/Rhea/GUI/Widget.h"
#include "GraphicsComponent.h"

namespace Rhea
{
	Poseidon::Node* GraphicsManager::createRenderable(GraphicsRenderElement* elem, Poseidon::Core* core, Unit* item, std::string render_context_name, bool dynamic_usage)
	{
		Poseidon::SceneManager* scene_mgr = core->get_scene_manager();

		// get mesh if already defined
		Poseidon::Mesh* mesh = elem->getMesh();
		if (!mesh) {
			// create an empty mesh
			mesh = new Poseidon::Mesh(dynamic_usage);
			mesh->setViewport(scene_mgr
				->getViewport());
			elem->SetMesh(mesh);
		}
		// create an entity
		Poseidon::Node* entity = core->createEntity(mesh);	
		elem->SetRenderEntity(entity);		
		
		Poseidon::Viewport* viewport = scene_mgr->getViewport();
		if (!viewport) {
			LOG(LOG_ERROR) << "No root viewport found - cannot create element";
			return nullptr;
		}
		Poseidon::RenderContext* render_context = viewport->getRenderContext(render_context_name);
		if (!render_context) {
			LOG(LOG_ERROR) << "GraphicsText::createText: render context '" << render_context_name << "' not found";
			return nullptr;
		}
		elem->setRenderContext(render_context);
		//render_context->addRenderChild(static_cast<Poseidon::Entity*>(entity));
		//render_context->addRenderChild((Item*)item);

		return entity;
	}

	Poseidon::Node* GraphicsManager::createEntity(GraphicsRenderElement* elem, Poseidon::Core* core, Unit* item, std::string render_context_name, bool dynamic_usage)
	{
		Poseidon::SceneManager* scene_mgr = core->get_scene_manager();

		// create an entity
		Poseidon::Node* entity = core->createEntity(nullptr);
		elem->SetRenderEntity(entity);

		Poseidon::Viewport* viewport = scene_mgr->getViewport();
		if (!viewport) {
			LOG(LOG_ERROR) << "No root viewport found - cannot create element";
			return nullptr;
		}
		Poseidon::RenderContext* render_context = viewport->getRenderContext(render_context_name);
		if (!render_context) {
			LOG(LOG_ERROR) << "GraphicsText::createText: render context '" << render_context_name << "' not found";
			return nullptr;
		}
		elem->setRenderContext(render_context);

		return entity;
	}

	void GraphicsManager::removeRenderable(Item* item)
	{
		GraphicsComponent* g_component = item->getComponent<GraphicsComponent>();
		if (g_component) {
			GraphicsRenderElement* elem = (GraphicsRenderElement*)g_component->getElement();
			Poseidon::RenderContext* render_context = elem->getRenderContext();
			if (!render_context) {
				LOG(LOG_ERROR) << "GraphicsText::removeRenderable: render context not set";
				return;
			}
			render_context->removeRenderChild(static_cast<Item*>(item));
		}
	}

	void GraphicsManager::setPosition(Unit* u, Transform* t)
	{
		Variant pos_sit = u->getValue("position");
		if (pos_sit != Variant()) {
			float x = pos_sit[0];
			float y = pos_sit[1];
			float z = 0.0f;
			if (pos_sit.size() > 2) {
				z = pos_sit[2];
			}

			t->get_position()->setPosition(x, y, z);
		}
		else
			t->get_position()->setPosition(0.0f, 0.0f, 0.0f);
	}

	void GraphicsManager::setMaterial(Poseidon::Core* core, Material* material)
	{
		// get the material element
		if (!material->getComponent<GraphicsComponent>()) {
			// create it in material manager
			std::string mat_name = material->getPathName();
			GraphicsMaterialManager* mat_mgr = static_cast<GraphicsMaterialManager*>(m_service->getManager<Material>());
			GraphicsMaterial* mat_elem = static_cast<GraphicsMaterial*>(mat_mgr->createElement(core, material));
		}
	}

	bool GraphicsManager::onHide(Event* ev, Unit* unit)
	{
		GraphicsComponent* g_component = unit->getComponent<GraphicsComponent>();
		if (g_component) {
			GraphicsRenderElement* elem = static_cast<GraphicsRenderElement*>(g_component->getElement());
			Poseidon::Entity* entity = static_cast<Poseidon::Entity*>(elem->getRenderEntity());
			entity->hide();
		}
		return true;
	}

	bool GraphicsManager::onShow(Event* ev, Unit* unit)
	{
		GraphicsComponent* g_component = unit->getComponent<GraphicsComponent>();
		if (g_component) {
			GraphicsRenderElement* elem = static_cast<GraphicsRenderElement*>(g_component->getElement());
			Poseidon::Entity* entity = static_cast<Poseidon::Entity*>(elem->getRenderEntity());
			entity->show();
		}
		return true;
	}
}