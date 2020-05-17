#ifndef GRAPHICS_SCENE_H
#define GRAPHICS_SCENE_H

#include <Poseidon/Core.h>

#include "../Element.h"
#include "../Scene.h"

namespace Rhea
{
	class GraphicsScene : public Element
	{
	public:
		GraphicsScene(Poseidon::Core* core) :
			m_core(core)			
		{}

		// create a texture from buffer
		void createScene(Scene* scene);

		static Element* createElement(Poseidon::Core* core, Unit* unit);

	protected:
		Poseidon::Core* m_core;
	};
}

#endif // !GRAPHICS_SCENE_H
