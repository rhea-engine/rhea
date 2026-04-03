#include "Poseidon/RenderContext.h"
//#include "Poseidon/Renderable.h"
#include "Poseidon/RenderQueue.h"
#include "Poseidon/Camera.h"
#include <Rhea/Components/MeshComponent.h>

namespace Poseidon
{
	RenderContext::RenderContext(std::string name) :
		m_camera(nullptr),
		m_name(name),
		m_stencil_state(0)
	{
		m_size = { 0 };
		m_size.max_depth = 1.0f;
	}

	void RenderContext::getLights(std::vector<Rhea::Light*>& lights)
	{
		for (std::vector<Item*>::iterator it = m_render_children.begin(); it != m_render_children.end(); ++it)
		{
			Rhea::Unit* robj = doCast<Rhea::Unit>(*it);
			if (robj) {
				robj->traverse([&](std::string name, Rhea::Component* uitem)->bool {
					Rhea::Light* light = doCast<Rhea::Light>(uitem);
					if (light)
						lights.push_back(light);
					return true;
					});
			}
		}
	}

	void RenderContext::cullObjects(RenderQueue* queue)
	{
		// cull pass
		// my children
		for (std::vector<Item*>::iterator it = m_render_children.begin(); it != m_render_children.end(); ++it)
		{
			Item* robj = (Item*)*it;

			// cull phase for top item robj
			robj->traverse([&](std::string name, Rhea::Component* uitem)->bool {
				Item* item = doCast<Item>(uitem);
				if (item) {
					Rhea::VisualComponent* renderer = item->getComponent<Rhea::VisualComponent>();
					if (renderer) {
						if (!renderer->isVisible())
							return false;
						Rhea::MeshComponent* mc = item->getComponent<Rhea::MeshComponent>();
						if (mc->getMesh())
							queue->addRenderable(item);
					}
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
