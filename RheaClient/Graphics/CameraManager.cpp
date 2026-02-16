#include <Poseidon/Viewport.h>
#include <Poseidon/Camera.h>
#include <Poseidon/Core.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/RenderContext.h>

#include "CameraManager.h"
#include <Rhea/Unit.h>
#include <Rhea/Fabrics/3D/Camera.h>
#include "RenderElement.h"
#include "GraphicsComponent.h"
#include <Psio/Log.h>

namespace Rhea
{
	GraphicsElement* GraphicsCameraManager::createElement(Poseidon::Core* core, Unit* unit)
	{	
		// check if already created
		GraphicsComponent* el = unit->getComponent<GraphicsComponent>();
		if (el)
			return el->getElement();

		// create
		GraphicsRenderElement* elem = new GraphicsRenderElement(core);
		Poseidon::SceneManager* m_scene_mgr = core->get_scene_manager();
		// get dimensions from viewport
		Poseidon::Viewport* viewport = m_scene_mgr->getViewport();
		if (!viewport) {
			LOG(LOG_ERROR) << "No root viewport found - cannot set camera";
			return nullptr;
		}
		float Width = (float)viewport->getWidth();
		float Height = (float)viewport->getHeight();
		if (!Width)
			Width = 800.0f;
		if (!Height)
			Height = 600.0f;

		// add a graphics sys camera
		Poseidon::Node* m_entity = core->createCamera();
		elem->SetRenderEntity(m_entity);

		// TODO get viewport from setup

		// get default viewport
		std::string render_context = "default";
		m_scene_mgr->getViewport()->getRenderContext(render_context)->setCamera(static_cast<Poseidon::Camera*>(m_entity));

		Math::Vector3 m_position;
		Variant pos_sit = unit->getValue("position");
		if (pos_sit != Variant()) {
			float x = pos_sit[0];
			float y = pos_sit[1];
			float z = pos_sit[2];
			m_position = Math::Vector3(x, y, z);
		}
		else
			m_position = Math::Vector3(0.0f, 0.0f, 0.0f);

		// set a default position
		((Camera*)unit)->getTransform()->get_position()->setPosition(m_position); 
		((Camera*)unit)->getTransform()->get_position()->lookAt(Math::Vector3(0.0f, 0.0f, 0.0f), Math::Vector3::UNIT_Z);
		m_entity->setTransform(((Item*)unit)->getTransform());

		static_cast<Poseidon::Camera*>(m_entity)->setAspectRatio((float)Width / (float)Height);

		unit->addComponentUnique<GraphicsComponent>(elem);

		return elem;
	}

	bool GraphicsCameraManager::onUpdate(Event* ev, Unit* unit)
	{
		// TODO (?) what could be an update?

		return false;
	}

	void GraphicsCameraManager::onFrame()
	{
	}

	bool GraphicsCameraManager::onItemDelete(Event* ev, Unit* unit)
	{
		return false;
	}
}
