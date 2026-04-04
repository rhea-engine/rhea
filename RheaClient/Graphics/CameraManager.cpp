#include <Poseidon/Viewport.h>
#include <Poseidon/Camera.h>
#include <Poseidon/Core.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/RenderContext.h>

#include <Rhea/Graphics/CameraManager.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Camera.h>
#include <Psio/Log.h>
#include <Rhea/Components/RenderComponent.h>

namespace rhea::client {
	bool GraphicsCameraManager::Ensure(poseidon::Core* core, rhea::Unit* unit)
	{	
		rhea::Camera* camera = doCast<rhea::Camera>(unit);
		if (!camera) return false;

		// check if already exists
		auto* rc = camera->getComponent<RenderComponent>();
		if (rc && rc->entity)	return true;

		if (!rc) rc = camera->addComponentUnique<RenderComponent>("renderer");
		
		// create
		
		poseidon::SceneManager* m_scene_mgr = core->get_scene_manager();
		// get dimensions from viewport
		poseidon::Viewport* viewport = m_scene_mgr->getViewport();
		if (!viewport) {
			LOG(LOG_ERROR) << "No root viewport found - cannot set camera";
			return false;
		}
		float Width = (float)viewport->getWidth();
		float Height = (float)viewport->getHeight();
		if (!Width)
			Width = 800.0f;
		if (!Height)
			Height = 600.0f;

		// add a graphics sys camera
		rc->entity = core->createCamera();
		
		// TODO get viewport from setup

		// get default viewport
		std::string render_context = "default";
		m_scene_mgr->getViewport()->getRenderContext(render_context)->setCamera(static_cast<poseidon::Camera*>(rc->entity));

		math::Vector3 m_position;
		Variant pos_sit = unit->getValue("position");
		if (pos_sit != Variant()) {
			float x = pos_sit[0];
			float y = pos_sit[1];
			float z = pos_sit[2];
			m_position = math::Vector3(x, y, z);
		}
		else
			m_position = math::Vector3(0.0f, 0.0f, 0.0f);

		// set a default position
		camera->getTransform()->get_position()->setPosition(m_position); 
		camera->getTransform()->get_position()->lookAt(math::Vector3(0.0f, 0.0f, 0.0f), math::Vector3::UNIT_Z);
		rc->entity->setTransform(((rhea::Item* )unit)->getTransform());

		static_cast<poseidon::Camera*>(rc->entity)->setAspectRatio((float)Width / (float)Height);

		// register handlers
		unit->registerEventHandler("delete", this, std::bind(&GraphicsCameraManager::onItemDelete, this, std::placeholders::_1, std::placeholders::_2));

		return true;
	}

	bool GraphicsCameraManager::onUpdate(rhea::Event* ev, rhea::Unit* unit)
	{
		// TODO (?) what could be an update?

		return false;
	}

	void GraphicsCameraManager::onFrame()
	{
	}

	bool GraphicsCameraManager::onItemDelete(rhea::Event* ev, rhea::Unit* unit)
	{
		rhea::Camera* item = doCast<rhea::Camera>(unit);
		if (!item) return true;

		auto* rc = item->getComponent<RenderComponent>();
		if (rc) {

			if (rc->ctx)
				rc->ctx->removeRenderChild(item);

			if (rc->entity)
				delete rc->entity;

			rc->entity = nullptr;
		}
		return false;
	}
}
