#include <Poseidon/Viewport.h>
#include <Poseidon/Camera.h>
#include <Poseidon/Core.h>
#include <Poseidon/SceneManager.h>

#include "Camera.h"
#include "../Unit.h"
#include "../Camera.h"

namespace Rhea
{
	void GraphicsCamera::createCamera(Camera* cam)
	{
		Poseidon::SceneManager* m_scene_mgr = m_core->get_scene_manager();
		float Width = 800;
		float Height = 600;

		// add a graphics sys camera
		m_entity = m_core->createCamera();

		// TODO get viewport from setup

		// get default viewport
		m_scene_mgr->getViewport()->addChild(m_entity);
		m_scene_mgr->getViewport()->setCamera(static_cast<Poseidon::Camera*>(m_entity));

		Poseidon::Vector3 m_position;
		Variant pos_sit = cam->getValue("position");
		if (pos_sit != Variant()) {
			float x = pos_sit[0];
			float y = pos_sit[1];
			float z = pos_sit[2];			
			m_position = Poseidon::Vector3(x, y, z);
		}
		else
			m_position = Poseidon::Vector3(0.0f, 0.0f, 0.0f);

		// set a default position
		m_entity->get_position()->setPosition(m_position);
		m_entity->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);

		static_cast<Poseidon::Camera*>(m_entity)->setAspectRatio((float)Width / (float)Height);

		cam->addElement("graphics", this);
	}	

	Element* GraphicsCamera::createElement(Poseidon::Core* core, Unit* unit)
	{	
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		// create
		GraphicsCamera* cam = new GraphicsCamera(core);
		cam->createCamera(static_cast<Camera*>(unit));

		return cam;
	}
}
