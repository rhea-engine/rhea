#include "Viewport.h"
#include "Mesh.h"
#include "Animatable.h"
#include "RenderQueue.h"
#include "Camera.h"

#include "Entity.h"
#include "Hippios/D3D11Renderer/D3D11Renderer.h"

namespace Poseidon
{	
	Viewport::Viewport()
	{
		m_groups.emplace("renderable", std::vector<Node*>());
	}
	
	void Viewport::cullObjects(RenderQueue* queue)
	{
		// cull pass
		// my children
		for (std::vector<Renderable*>::iterator it = m_render_children.begin(); it != m_render_children.end(); ++it)
		{
			Renderable* robj = (Renderable*)*it;
			robj->cullPhase(queue, m_camera);
		}
	}

	void Viewport::initRenderTarget(Hippios::Engine* engine)
	{
		m_engine = engine;
		m_renderer = m_engine->getRenderer();
	}

	void Viewport::setCamera(Camera* camera)
	{
		m_camera = camera;
	}

	void Viewport::registerNode()
	{
		registerGroup("viewport", this);
	}

	void Viewport::registerGroup(std::string group_name, Node* node)
	{
		if (group_name == "renderable") {
			m_groups[group_name].push_back(node);
			return; // dont register above this viewport
		}

		Node::registerGroup(group_name, node);
	}

	void Viewport::addRenderChild(Renderable* child)
	{
		Node::addChild(child);
		m_render_children.push_back(child);
	}
}