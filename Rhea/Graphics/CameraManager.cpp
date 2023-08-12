#include <Poseidon/Viewport.h>
#include <Poseidon/Camera.h>
#include <Poseidon/Core.h>
#include <Poseidon/SceneManager.h>
#include <Poseidon/RenderContext.h>

#include "CameraManager.h"
#include "../Unit.h"
#include "Fabrics/3D/Camera.h"
#include "RenderElement.h"

namespace Rhea
{
	Element* GraphicsCameraManager::createElement(Poseidon::Core* core, Unit* unit)
	{	
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		// create
		GraphicsRenderElement* elem = new GraphicsRenderElement(core);
		Poseidon::SceneManager* m_scene_mgr = core->get_scene_manager();
		// get dimensions from viewport
		Poseidon::Viewport* viewport = m_scene_mgr->getViewport();
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

		unit->addElement("graphics", elem);

		return elem;
	}

	bool GraphicsCameraManager::onUpdate(Event* ev, Unit* unit)
	{
		printf("item updated\n");

/*		Item* item = doCast<Item>(unit);
		if (item)
			GraphicsRenderElement* elem = static_cast<GraphicsRenderElement*>(item->getElement("graphics"));
*/

		// TODO (?) what could be an update?

		return false;
	}

	void GraphicsCameraManager::onFrame()
	{
	}

	bool GraphicsCameraManager::onItemDelete(Event* ev, Unit* unit)
	{
		//erase element from list
		Element* elem = unit->getElement("graphics");
		unit->addElement("graphics", nullptr);
		// free element
		delete elem;

		return false;
	}
}
