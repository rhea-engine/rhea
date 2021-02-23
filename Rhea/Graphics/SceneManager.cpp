#include <Poseidon/SceneManager.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
#include <Poseidon/Math/Matrix3x4.h>
#include <Poseidon/Math/Matrix4.h>

#include "SceneManager.h"
#include "ItemManager.h"
#include "CameraManager.h"
#include "../Unit.h"
#include "../Item.h"
#include "Fabrics/Canvas/Item2D.h"
#include "GraphicsService.h"

namespace Rhea
{
	void GraphicsSceneManager::createScene(Poseidon::Core* core, GraphicsScene *elem, Scene* scene)
	{
		Poseidon::SceneManager* scene_mgr = core->get_scene_manager();

		// add my items
		std::vector<Item*> items = scene->getItems();
		for (auto it = items.begin(); it != items.end(); ++it) {
			Item* item = *it;
			printf("typename: %s\n", item->getTypeName().c_str());

			// try manager
			GraphicsManager* mgr = m_service->getManager(item->getTypeName());
			if (mgr) {
				mgr->createElement(core, item);
			}
			else {
				mgr = m_service->getManager("//meworla/item");
				mgr->createElement(core, item);
			}
		}
		scene->addElement("graphics", elem);
	}

	Element* GraphicsSceneManager::createElement(Poseidon::Core* core, Unit* unit)
	{
		// check if already created
		Element* el = unit->getElement("graphics");
		if (el)
			return el;

		// create it
		GraphicsScene* scene_elem = new GraphicsScene();
		createScene(core, scene_elem, static_cast<Scene*>(unit));

		return scene_elem;
	}
}