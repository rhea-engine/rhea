#ifndef RHEACLIENT_GRAPHICSCOMPONENT_H
#define RHEACLIENT_GRAPHICSCOMPONENT_H

#include <Poseidon/RenderTypes.h>
#include <Poseidon/RenderContext.h>

#include <Rhea/Component.h>

namespace rhea::client {
	class GraphicsService;
	class GraphicsComponent : public rhea::Component
	{
		RECLASS(GraphicsComponent, Component, "//meworla/graphicscomponent")
	public:
		GraphicsComponent(std::string name, std::string type_name) : Component(name, type_name) {}
		GraphicsComponent(std::string name, poseidon::GraphicsNodeId id) :
			GraphicsComponent(name, "//meworla/graphicscomponent")
		{
			gfx_node_id = id;
		}
		
		poseidon::RenderContext* ctx = nullptr;

		poseidon::GraphicsNodeId gfx_node_id = poseidon::InvalidGraphicsNode;
	};
}
#endif // !RHEACLIENT_GRAPHICSCOMPONENT_H
