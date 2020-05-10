#include <Poseidon/Viewport.h>
#include <Poseidon/Camera.h>
#include <Poseidon/Core.h>
#include <Poseidon/SceneManager.h>
#include "Application.h"

#include "SceneView.h"

namespace Rhea
{
	void SceneView::show(Poseidon::Core* core)
	{
		Poseidon::SceneManager* m_scene_mgr = core->get_scene_manager();
		float Width = 800;
		float Height = 600;

		// add a graphics sys camera
		m_camera = core->createCamera();

		// TODO get viewport from setup

		// get default viewport
		m_scene_mgr->getViewport()->addChild(m_camera);
		m_scene_mgr->getViewport()->setCamera(m_camera);

		auto it = m_values.find("position");
		Poseidon::Vector3 m_position;
		if (it != m_values.end())
			m_position = Poseidon::Vector3(it->second[0], it->second[1], it->second[2]);
		else
			m_position = Poseidon::Vector3(0.0f, 0.0f, 0.0f);

		// set a default position
		m_camera->get_position()->setPosition(m_position);
		m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);

		m_camera->setAspectRatio((float)Width / (float)Height);
	}

	void SceneView::load(ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			return handler(ReqError::OK);

		if (m_status == Status::SETUP) {
			m_status = Status::LOADED;
			show(m_app->get_core());
			return handler(ReqError::OK);
		}
		else {
			// TODO load myself with other means
			printf("SceneView::load without setup not implemented\n");
		}

	}

	void SceneView::message(std::string msg)
	{
		Unit::message(msg);
		if (msg == "show") {
			show(m_app->get_core());
		}
	}

	Poseidon::Node* SceneView::getNode()
	{
		return m_camera;
	}

	void SceneView::setPosition(Poseidon::Position& position)
	{
		if (!m_camera)
			return m_app->log("SceneView:setPosition called before camera was set");
		m_camera->get_position()->setPosition(position);
	}

	void SceneView::setPosition(Poseidon::Vector3 position)
	{
		if (!m_camera)
			return m_app->log("SceneView:setPosition called before camera was set");
		m_camera->get_position()->setPosition(position);
	}

	void SceneView::setLookAt(Poseidon::Vector3 look_at)
	{
		if (!m_camera)
			return m_app->log("SceneView:setLookAt called before camera was set");
		m_camera->get_position()->lookAt(look_at);
	}

	Unit* SceneView::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new SceneView(name, type);
		return ret;
	}
}
