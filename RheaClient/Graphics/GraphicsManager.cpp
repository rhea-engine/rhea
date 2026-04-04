#include <Poseidon/Core.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Entity.h>
#include <Poseidon/Mesh.h>
#include <Poseidon/RenderContext.h>

#include <Rhea/Application.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Material.h>

#include <Rhea/Graphics/GraphicsManager.h>
#include <Rhea/Graphics/MaterialManager.h>
#include <Rhea/Graphics/GraphicsService.h>
#include <Psio/Log.h>
#include <Rhea/GUI/Widget.h>

namespace rhea::client {
	poseidon::Node* GraphicsManager::createRenderable(RenderComponent* rc, poseidon::Core* core, rhea::Unit* item, poseidon::Mesh* mesh, std::string render_context_name)
	{
		poseidon::SceneManager* scene_mgr = core->get_scene_manager();
		
		// create an entity using the shared mesh
		poseidon::Node* entity = core->createEntity(mesh);	
		rc->entity = entity;
		
		poseidon::Viewport* viewport = scene_mgr->getViewport();
		if (!viewport) {
			LOG(LOG_ERROR) << "No root viewport found - cannot create element";
			return nullptr;
		}
		poseidon::RenderContext* render_context = viewport->getRenderContext(render_context_name);
		if (!render_context) {
			LOG(LOG_ERROR) << "GraphicsText::createText: render context '" << render_context_name << "' not found";
			return nullptr;
		}
		rc->ctx = render_context;
		//render_context->addRenderChild(static_cast<poseidon::Entity*>(entity));
		//render_context->addRenderChild((rhea::Item* )item);

		return entity;
	}

	poseidon::Node* GraphicsManager::createEntity(RenderComponent* rc, poseidon::Core* core, rhea::Unit* item, std::string render_context_name, bool dynamic_usage)
	{
		poseidon::SceneManager* scene_mgr = core->get_scene_manager();

		// create an entity
		poseidon::Node* entity = core->createEntity(nullptr);
		rc->entity = entity;

		poseidon::Viewport* viewport = scene_mgr->getViewport();
		if (!viewport) {
			LOG(LOG_ERROR) << "No root viewport found - cannot create element";
			return nullptr;
		}
		poseidon::RenderContext* render_context = viewport->getRenderContext(render_context_name);
		if (!render_context) {
			LOG(LOG_ERROR) << "GraphicsText::createText: render context '" << render_context_name << "' not found";
			return nullptr;
		}
		rc->ctx = render_context;

		return entity;
	}

	void GraphicsManager::setPosition(rhea::Unit* u, rhea::Transform* t)
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