#include "Scene.h"

#include <Poseidon/SceneManager.h>
#include <Poseidon/Material.h>
#include <Poseidon/Math/Matrix3x4.h>
#include <Poseidon/Math/Matrix4.h>

#include "Item.h"
#include "Camera.h"
#include "../Unit.h"
#include "../Item.h"

namespace Rhea
{
	struct cbPerObjectMVP
	{
		Poseidon::Matrix3x4  model;
		Poseidon::Matrix4 vp;
	};

	void GraphicsScene::createScene(Scene* scene)
	{
		Poseidon::SceneManager* scene_mgr = m_core->get_scene_manager();
		float Width = 800;
		float Height = 600;

		// prepare shaders		
		Poseidon::Material* material = m_core->createMaterial();
		material->setType(1);
		if (material->loadFromFile("Samples\\assets\\shaders\\uvshader_vp.hlsl", "vs", "VShader"))
			exit(-1);
		if (material->loadFromFile("Samples\\assets\\shaders\\uvshader_vp.hlsl", "ps", "PShader"))
			exit(-1);
		material->initBuffer(sizeof(cbPerObjectMVP));

		// add my items
		std::vector<Item*> items = scene->getItems();
		for (auto it = items.begin(); it != items.end(); ++it) {
			Item* item = *it;
			if (item->getType() == "//meworla/camera")
				GraphicsCamera::createElement(m_core, item);
			else
				GraphicsItem::createElement(m_core, item);
		}
		scene->addElement("graphics", this);
	}

	Element* GraphicsScene::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		GraphicsScene* scene = new GraphicsScene(core);
		scene->createScene(static_cast<Scene*>(unit));

		return scene;
	}
}