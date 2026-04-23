#include "Poseidon/RenderContext.h"
//#include "Poseidon/Renderable.h"
#include "Poseidon/RenderQueue.h"
#include "Poseidon/Camera.h"
#include <Rhea/Components/MeshComponent.h>
#include <RheaClient/Components/RenderComponent.h>

namespace poseidon
{
	RenderContext::RenderContext(std::string name) :		
		m_name(name),
		m_stencil_state(0)
	{
		m_size = { 0 };
		m_size.max_depth = 1.0f;
	}

	void RenderContext::addLight(LightHandle handle)
	{
		m_lights.push_back(handle);
	}

	void RenderContext::removeLight(LightHandle handle)
	{
		// remove light from light-list
		m_lights.erase(std::remove(m_lights.begin(), m_lights.end(), handle), m_lights.end());
	}

	void RenderContext::cullObjects(RenderQueue* queue)
	{
		if (m_render_order_mode == RenderOrderMode::TreeOrdered) {
			for(auto root : m_render_roots) {
				cullOrderedRecursive(root, queue);
			}
		}
		else {
			for (std::vector<Item*>::iterator it = m_render_children.begin(); it != m_render_children.end(); ++it)
			{
				Item* robj = (Item*)*it;
				
				if (!robj)
					continue;

				// cull phase for top item robj
				robj->traverse([&](std::string name, rhea::Component* uitem)->bool {
					Item* item = doCast<Item>(uitem);
					if (item) {
							
						rhea::VisualComponent* renderer = item->getComponent<rhea::VisualComponent>();
						if (renderer) {
							if (!renderer->isVisible())
								return false;
							rhea::MeshComponent* mc = item->getComponent<rhea::MeshComponent>();
							auto* rc = item->getComponent<rhea::client::RenderComponent>();
							if (mc && mc->getMesh() && rc && rc->hasRenderable())
								queue->addRenderable(rc->renderable);
						}
						return true;
					}
					else {
						// not an item - try children
						return true;
					}
				});
			}
		}
	}

	void RenderContext::cullOrderedRecursive(Item* item, RenderQueue* queue)
	{
		if (!item)
			return;

		rhea::VisualComponent* renderer = item->getComponent<rhea::VisualComponent>();
		if (renderer && !renderer->isVisible())
			return;

		rhea::MeshComponent* mc = item->getComponent<rhea::MeshComponent>();
		auto* rc = item->getComponent<rhea::client::RenderComponent>();
		if (mc && mc->getMesh() && rc && rc->hasRenderable())
			queue->addRenderable(rc->renderable);

		item->children([&](std::string name, rhea::Component* c)->bool {
			Item* child = doCast<Item>(c);
			if (child) {
				cullOrderedRecursive(child, queue);
				return true;
			}
			else {
				// not an item - try children
				return true;
			}
		}, true);
	}	

	bool RenderContext::containsRenderChild(Item* item) const
	{
		return std::find(m_render_children.begin(), m_render_children.end(), item) != m_render_children.end();
	}

	bool RenderContext::isRootInContext(Item* item) const
	{
		if (!item)
			return false;

		Item* parent = doCast<Item>(item->getParent());
		if (!parent)
			return true;

		return !containsRenderChild(parent);
	}

	void RenderContext::addRenderChild(Item* child)
	{
		if (!child)
			return;

		// Avoid adding the same child multiple times
		if (containsRenderChild(child))
			return;

		// Add child to render children
		m_render_children.push_back(child);

		if (m_render_order_mode == RenderOrderMode::Insertion)
			return;

		// If parent is not in context, this is a root
		if (isRootInContext(child))
			m_render_roots.push_back(child);
		// Any existing descendants that were roots are no longer roots
		child->traverse([&](std::string name, rhea::Component* c)->bool {
			Item* childitem = doCast<Item>(c);
			if (!childitem || childitem == child)
				return true;

			if (containsRenderChild(childitem))
				m_render_roots.erase(std::remove(m_render_roots.begin(), m_render_roots.end(), childitem), m_render_roots.end());

			return true;
			});
	}

	void RenderContext::AddRenderable(RenderableHandle handle)
	{
		m_renderables.push_back(handle);
	}

	void RenderContext::removeRenderChild(Item* child)
	{
		if (!child)
			return;

		auto it = std::find(m_render_children.begin(), m_render_children.end(), child);
		if (it != m_render_children.end())
			m_render_children.erase(it);

		// Only update roots in TreeOrdered mode
		if (m_render_order_mode == RenderOrderMode::Insertion)
			return;

		// Descendants still in context may become roots now
		child->traverse([&](std::string name, rhea::Component* c)->bool {
			Item* childItem = doCast<Item>(c);
			if (!childItem || childItem == child)
				return true;

			if (containsRenderChild(childItem) && isRootInContext(childItem)) {
				if (std::find(m_render_roots.begin(), m_render_roots.end(), childItem) == m_render_roots.end())
					m_render_roots.push_back(childItem);
			}

			return true;
		});
	}

	void RenderContext::RemoveRenderable(RenderableHandle handle)
	{
		m_renderables.erase(std::remove(m_renderables.begin(), m_renderables.end(), handle), m_renderables.end());
	}

	void RenderContext::rebuildRoots()
	{
		m_render_roots.clear();

		for (Item* item : m_render_children) {
			if (isRootInContext(item))
				m_render_roots.push_back(item);
		}
	}
}
