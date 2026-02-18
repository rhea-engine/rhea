#include <Poseidon/Core.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Entity.h>
#include <Poseidon/Mesh.h>
#include <Poseidon/RenderContext.h>

#include <Rhea/Application.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Material.h>

#include "GraphicsManager.h"
#include "MaterialManager.h"
#include "GraphicsService.h"
#include <Psio/Log.h>
#include <Rhea/GUI/Widget.h>

namespace Rhea
{
	Poseidon::Node* GraphicsManager::createRenderable(RenderComponent* rc, Poseidon::Core* core, Unit* item, Poseidon::Mesh* mesh, std::string render_context_name)
	{
		Poseidon::SceneManager* scene_mgr = core->get_scene_manager();
		
		// create an entity using the shared mesh
		Poseidon::Node* entity = core->createEntity(mesh);	
		rc->entity = entity;
		
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
		rc->ctx = render_context;
		//render_context->addRenderChild(static_cast<Poseidon::Entity*>(entity));
		//render_context->addRenderChild((Item*)item);

		return entity;
	}

	Poseidon::Node* GraphicsManager::createEntity(RenderComponent* rc, Poseidon::Core* core, Unit* item, std::string render_context_name, bool dynamic_usage)
	{
		Poseidon::SceneManager* scene_mgr = core->get_scene_manager();

		// create an entity
		Poseidon::Node* entity = core->createEntity(nullptr);
		rc->entity = entity;

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
		rc->ctx = render_context;

		return entity;
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
}