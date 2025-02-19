#include <algorithm>

#include "Viewport.h"
#include "Mesh.h"
#include "Animatable.h"
#include "RenderContext.h"

#include "Entity.h"
#include "Hippios/D3D11Renderer/D3D11Renderer.h"

namespace Poseidon
{	
	Viewport::Viewport(std::string name) :
		m_name(name),
		m_width(-1),
		m_height(-1),
		m_swapchain(nullptr)
	{
		m_groups.emplace("renderable", std::vector<Node*>());
	}

	Viewport::~Viewport()
	{
		Node::~Node();
	}

	void Viewport::initRenderTarget(Hippios::Engine* engine)
	{
		m_engine = engine;
		m_renderer = m_engine->getRenderer();
	}

	RenderContext* Viewport::getRenderContext(std::string& name)
	{
		for (auto it = m_render_contexts.begin(); it != m_render_contexts.end(); ++it)
		{
			RenderContext* context = (*it).get();
			if (context->isName(name))
				return context;
		}
		return nullptr;
	}

	RenderContext* Viewport::createRenderContext(std::string& name)
	{
		m_render_contexts.push_back(std::make_unique<RenderContext>(name));
		return m_render_contexts.back().get();
	}

	void Viewport::createRenderTarget(int width, int height, Hippios::SwapChain* swap_chain)
	{
		m_width = width;
		m_height = height;
		m_swapchain = swap_chain;
		m_render_target.reset(m_renderer->createRenderTarget(width, height, swap_chain));
		// create a default context
		m_render_contexts.clear();
		std::string name = "default";
		RenderContext* context = createRenderContext(name);
		context->setSize((float)width, (float)height);
	}

	void Viewport::onResize(int width, int height)
	{
		if (m_width == width && m_height == height)
			return;

		m_width = width;
		m_height = height;
		// need to update render target
		if (m_render_target)
			m_render_target->create(m_width, m_height, m_swapchain);		
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