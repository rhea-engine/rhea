#ifndef RHEA_SCENE_H
#define RHEA_SCENE_H

#include <vector>

#include "Resource.h"

namespace Poseidon
{
	class Core;
	class Camera;
	class SceneManager;
}

namespace Rhea
{
	class Item;

	// a scene is a holder for various corresponding scene objects
	class Scene : public Resource
	{
	public:
		Scene(std::string name) :
			m_scene_mgr(NULL),
			m_camera(NULL),
			Resource(name, "//meworla/scene")
		{}

		// show the scene in the viewport
		void show(Poseidon::Core* core);

		// add an item to the scene
		void addItem(Item* item);

		void load(ReadyHandler handler);
	protected:
		std::vector<Item*> m_items;
		Poseidon::SceneManager* m_scene_mgr;
		Poseidon::Camera* m_camera;
	};
}

#endif // !RHEA_SCENE_H
