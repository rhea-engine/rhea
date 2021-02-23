#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include <Poseidon/Core.h>

#include "../Element.h"
#include "../Scene.h"

#include "GraphicsManager.h"

namespace Rhea
{
	class GraphicsScene : public Element
	{

	};

	class GraphicsSceneManager : public GraphicsManager
	{
	public:
		GraphicsSceneManager(GraphicsService* service) :
			GraphicsManager(service)
		{}

		void createScene(Poseidon::Core* core, GraphicsScene* elem, Scene* scene);

		Element* createElement(Poseidon::Core* core, Unit* unit);	

		void onFrame() {}
	};
}

#endif // !GRAPHICS_SCENE_H
