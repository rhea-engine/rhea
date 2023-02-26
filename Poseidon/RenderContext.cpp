#include "RenderContext.h"
//#include "Renderable.h"
#include "RenderQueue.h"
#include "Camera.h"
#include <Rhea/Graphics/RenderElement.h>
#include <Rhea/Components/RenderComponent.h>

namespace Poseidon
{
	RenderContext::RenderContext(std::string& name) :
		m_camera(nullptr),
		m_name(name),
		m_stencil_state(0)
	{
	}

	void RenderContext::cullObjects(RenderQueue* queue)
	{
		// cull pass
		// my children
		for (std::vector<Item*>::iterator it = m_render_children.begin(); it != m_render_children.end(); ++it)
		{
			Item* robj = (Item*)*it;

			// cull phase for top item robj
			robj->traverse([&](std::string name, Rhea::Unit* uitem)->bool {
				if (uitem->getValue("isItem")) {
					Item* item = (Item*)uitem;
					Rhea::RenderComponent* render_com = item->getGraphicsComponent();
					if (!render_com)
						return true;
					if (!render_com->isVisible())
						return false;
					if (render_com->getMesh())
						queue->addRenderable(item);
					return true;
				}
				else {
					// not an item - try children (note we can start with scene like this)
					return true;
				}
				});

		//	robj->cullPhase(queue, m_camera);
		}
	}

	void RenderContext::setCamera(Camera* camera)
	{
		m_camera = camera;
	}

	void RenderContext::addRenderChild(Item* child)
	{
		m_render_children.push_back(child);
	}

	void RenderContext::removeRenderChild(Item* child)
	{
		auto it = std::find(m_render_children.begin(), m_render_children.end(), child);
		m_render_children.erase(it);
	}
}
