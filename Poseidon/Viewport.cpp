#include <algorithm>

#include "Viewport.h"
#include "Mesh.h"
#include "Animatable.h"
#include "RenderContext.h"

#include "Entity.h"
#include "Hippios/D3D11Renderer/D3D11Renderer.h"

namespace Poseidon
{	
	Viewport::Viewport(std::string& name) :
		m_name(name)
	{
		m_groups.emplace("renderable", std::vector<Node*>());
		RenderContext* context = new RenderContext(std::string("default"));
		m_render_contexts.push_back(context);
		Node::addChild(context);
	}

	Viewport::~Viewport()
	{
		Node::~Node();
		for (std::vector<RenderContext*>::iterator it = m_render_contexts.begin(); it != m_render_contexts.end(); ++it)
		{
			RenderContext* context = *it;
			delete context;
		}
	}

	void Viewport::initRenderTarget(Hippios::Engine* engine)
	{
		m_engine = engine;
		m_renderer = m_engine->getRenderer();
	}

	RenderContext* Viewport::getRenderContext(std::string& name)
	{
		for (std::vector<RenderContext*>::iterator it = m_render_contexts.begin(); it != m_render_contexts.end(); ++it)
		{
			RenderContext* context = *it;
			if (context->isName(name))
				return context;
		}
		return nullptr;
	}

	RenderContext* Viewport::createRenderContext(std::string& name)
	{
		RenderContext* context = new RenderContext(name);
		m_render_contexts.push_back(context);
		Node::addChild(context);
		return context;
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
}