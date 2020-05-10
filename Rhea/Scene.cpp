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
	}

	void Scene::addItem(Item* item)
	{
		m_items.push_back(item);
	}
	
	void Scene::load(ReadyHandler handler)
	{
		if (m_status >= Status::LOADED)
			handler(ReqError::OK);

		int to_load = (int)m_items.size();
		if (!to_load) {
			m_status = Status::LOADED;
			handler(ReqError::OK);
		}

		// iterate over children and load them
		// call ready handler when all children where loaded	
		// this ignores errors in loading children 
		// TODO check if this should fail fast instead of ignore?
		// Note: handlers and loaders may add items to the list
		int item_size = m_items.size();
		for (int i = 0; i < item_size; i++) {
			Item* item = m_items[i];
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