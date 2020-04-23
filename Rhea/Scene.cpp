#include "Scene.h"
#include "Item.h"
#include "Poseidon/Core.h"
#include "Poseidon/SceneManager.h"
#include "Poseidon/Material.h"
#include "Poseidon/Camera.h"
#include "Poseidon/Position.h"
#include "Poseidon/Viewport.h"
#include "Poseidon/Math/Matrix3x4.h"
#include "Poseidon/Math/Matrix4.h"

namespace Rhea
{
	struct cbPerObjectMVP
	{
		Poseidon::Matrix3x4  model;
		Poseidon::Matrix4 vp;
	};

	void Scene::show(Poseidon::Core* core)
	{
		m_scene_mgr = core->get_scene_manager();
		float Width = 800;
		float Height = 600;

		// prepare shaders		
		Poseidon::Material* material = core->createMaterial();
		material->setType(1);
		if (material->loadFromFile("Samples\\assets\\shaders\\uvshader_vp.hlsl", "vs", "VShader"))
			exit(-1);
		if (material->loadFromFile("Samples\\assets\\shaders\\uvshader_vp.hlsl", "ps", "PShader"))
			exit(-1);
		material->initBuffer(sizeof(cbPerObjectMVP));

		// add my items
		for (auto it = m_items.begin(); it != m_items.end(); ++it) {
			Item* item = *it;
			item->show(core);
		}

		// add a camera
		m_camera = core->createCamera();
		m_scene_mgr->getViewport()->addChild(m_camera);
		m_scene_mgr->getViewport()->setCamera(m_camera);

		Poseidon::Vector3 m_position = Poseidon::Vector3(0.0f, 3.0f, -10.0f);

		// set a default position
		m_camera->get_position()->setPosition(m_position);
		m_camera->get_position()->lookAt(Poseidon::Vector3(0.0f, 0.0f, 0.0f), Poseidon::Vector3::UNIT_Z);

		m_camera->setAspectRatio((float)Width / (float)Height);
	}

	void Scene::addItem(Item* item)
	{
		m_items.push_back(item);
	}
	
	void Scene::load(ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			handler(ReqError::OK);

		int to_load = m_items.size();
		if (!to_load) {
			m_status = Status::LOADED;
			handler(ReqError::OK);
		}

		// iterate over children and load them
		// call ready handler when all children where loaded	
		// this ignores errors in loading children 
		// TODO check if this should fail fast instead of ignore?
		for (auto it = m_items.begin(); it != m_items.end(); ++it) {
			Item* item = *it;
			item->load([&](ReqError err) {
				to_load--;
				if (to_load <= 0) {
					m_status = Status::LOADED;
					handler(ReqError::OK);
				}
				});
		}

	}
}