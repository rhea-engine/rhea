#include <Poseidon/Core.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/Entity.h>
#include <Poseidon/Mesh.h>
#include <Poseidon/RenderContext.h>
#include <Poseidon/RenderWorld.h>

#include <Rhea/Application.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Material.h>

#include <RheaClient/Graphics/GraphicsManager.h>
#include <RheaClient/Graphics/MaterialManager.h>
#include <RheaClient/Graphics/GraphicsService.h>
#include <Psio/Log.h>
#include <Rhea/GUI/Widget.h>

namespace rhea::client {
	poseidon::RenderableHandle GraphicsManager::createRenderable(RenderComponent* rc, poseidon::Core* core, rhea::Unit* item, poseidon::Mesh* mesh, std::string render_context_name)
	{
		if (!mesh) {
			LOG(LOG_ERROR) << "GraphicsManager::createRenderable: mesh is null";
			return poseidon::InvalidRenderable;
		}
		if (!rc) {
			LOG(LOG_ERROR) << "GraphicsManager::createRenderable: render component is null";
			return poseidon::InvalidRenderable;
		}

		poseidon::RenderContext* render_context = getRenderContext(core, render_context_name);
		if (!render_context)
			return poseidon::InvalidRenderable;

		// create a renderabke using the shared mesh
		rc->renderable = core->getRenderWorld()->createRenderable(mesh);
		rc->ctx = render_context;		

		return rc->renderable;
	}

	poseidon::RenderContext* GraphicsManager::getRenderContext(poseidon::Core* core, std::string render_context_name)
	{
		poseidon::SceneManager* scene_mgr = core->get_scene_manager();
		if (!scene_mgr) {
			LOG(LOG_ERROR) << "GraphicsManager::getRenderContext: no scene manager";
			return nullptr;
		}
		poseidon::Viewport* viewport = scene_mgr->getViewport();
		if (!viewport) {
			LOG(LOG_ERROR) << "No root viewport found - cannot get render context";
			return nullptr;
		}
		poseidon::RenderContext* render_context = viewport->getRenderContext(render_context_name);
		if (!render_context) {
			LOG(LOG_ERROR) << "GraphicsManager::getRenderContext: render context '" << render_context_name << "' not found";
			return nullptr;
		}
		return render_context;
	}	

	poseidon::CameraHandle GraphicsManager::createCamera(poseidon::Core* core)
	{
		return core->getRenderWorld()->createCamera();
	}

	poseidon::LightHandle GraphicsManager::createLight(poseidon::Core* core)
	{		
		return core->getRenderWorld()->createLight();
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