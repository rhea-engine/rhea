#include <algorithm>

#include "Poseidon/Viewport.h"
#include "Poseidon/Mesh.h"
#include "Poseidon/RenderContext.h"

namespace poseidon
{	
	Viewport::Viewport(std::string name) :
		m_name(name),
		m_x(0),
		m_y(0),
		m_width(0),
		m_height(0)
	{
		// default context
		std::string ctx_name = "default";
		createRenderContext(ctx_name);
	}

	Viewport::~Viewport() = default;
	
	RenderContext* Viewport::getRenderContext(std::string& name)
	{
		for (auto& rc : m_render_contexts) {
			if (rc->isName(const_cast<std::string&>(name)))
				return rc.get();
		}
		return nullptr;
	}

	RenderContext* Viewport::createRenderContext(std::string& name)
	{
		m_render_contexts.push_back(std::make_unique<RenderContext>(name));
		m_render_contexts.back()->setSize((float)m_width, (float)m_height);
		return m_render_contexts.back().get();
	}	

	void Viewport::setRect(int x, int y, int width, int height)
	{
		m_x = x;
		m_y = y;
		onResize(width, height);
	}

	void Viewport::onResize(int width, int height)
	{
		if (m_width == width && m_height == height)
			return;

		m_width = width;
		m_height = height;		

		// Update RenderContexts
		for (auto& context : m_render_contexts) {
			context->setSize((float)width, (float)height);
		}

		// TODO: Trigger callback/event to GraphicsService ?
	}
}