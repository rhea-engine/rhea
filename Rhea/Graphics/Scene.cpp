#include "Scene.h"

#include <Poseidon/SceneManager.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
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
		Poseidon::Matrix3x4 model;
		Poseidon::Matrix4 vp;
	};

	void GraphicsScene::createScene(Scene* scene)
	{
		Poseidon::SceneManager* scene_mgr = m_core->get_scene_manager();
		float Width = 800;
		float Height = 600;

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