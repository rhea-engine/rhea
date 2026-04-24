#include "Poseidon/RenderContext.h"
#include "Poseidon/Renderable.h"
#include "Poseidon/RenderQueue.h"
#include "Poseidon/Camera.h"
#include <Poseidon/RenderWorld.h>
#include <algorithm>
#include <unordered_set>

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

	void RenderContext::cullObjects(RenderQueue* queue, RenderWorld* world)
	{
		if (!queue)
			return;

		if (m_hierarchy_dirty)
			rebuildHierarchy();

		if (m_render_order_mode == RenderOrderMode::TreeOrdered) {
			for(auto root_id : m_root_nodes) {
				collectOrderedRecursive(root_id, queue, world);
			}			
		}
		else {
			for (auto handle : m_ordered_renderables)
				queue->addRenderable(handle);
		}
	}

	void RenderContext::collectOrderedRecursive(RenderNodeId node_id, RenderQueue* queue, RenderWorld* world)	
	{
		auto it = m_order_nodes.find(node_id);
		if (it == m_order_nodes.end())
			return;

		const auto& node = it->second;

		// check visibility of the renderable
		//poseidon::Renderable* renderable = world->resolveRenderable(node.renderable);
		//if (renderable && !renderable->visible)
		if (!node.visible)
			return;

		if (node.renderable != InvalidRenderable)
			queue->addRenderable(node.renderable);

		for (RenderNodeId child : node.children)
			collectOrderedRecursive(child, queue, world);
	}

	void RenderContext::rebuildHierarchy()
	{
		// Clear existing children relationships
		for (auto& [nodeId, node] : m_order_nodes)
			node.children.clear();

		m_root_nodes.clear();

		// Rebuild parent-child relationships
		for (auto& [nodeId, node] : m_order_nodes) {
			if (node.parent == InvalidRenderNode) {
				// No parent -> this is a root node
				m_root_nodes.push_back(nodeId);
			}
			else {
				// Has parent -> add to parent's children list
				auto parentIt = m_order_nodes.find(node.parent);
				if (parentIt != m_order_nodes.end()) {
					parentIt->second.children.push_back(nodeId);
				}
				else {
					// Parent not found -> treat as root
					m_root_nodes.push_back(nodeId);
				}
			}
		}

		// sort roots
		sortNodes(m_root_nodes);

		// sort each sibling list
		for (auto& [id, node] : m_order_nodes)
			sortNodes(node.children);

		m_hierarchy_dirty = false;
	}

	void RenderContext::sortNodes(std::vector<RenderNodeId>& nodes)
	{
		std::stable_sort(nodes.begin(), nodes.end(),
			[this](RenderNodeId a, RenderNodeId b) {
				const auto& na = m_order_nodes.at(a);
				const auto& nb = m_order_nodes.at(b);
				return na.sort_index < nb.sort_index;
			});
	}

	void RenderContext::setVisibility(RenderNodeId nodeId, bool mode)
	{
		auto it = m_order_nodes.find(nodeId);
		if (it == m_order_nodes.end())
			return;

		it->second.visible = mode;
	}

	RenderNodeId RenderContext::registerOrderNode(GraphicsNodeId sourceId, RenderNodeId parent)
	{
		auto existing = m_source_to_node.find(sourceId);
		if (existing != m_source_to_node.end()) {
			RenderNodeId id = existing->second;
			auto it = m_order_nodes.find(id);
			if (it != m_order_nodes.end()) {
				it->second.parent = parent;
				m_hierarchy_dirty = true;
				return id;
			}
		}

		RenderNodeId new_id = m_next_node_id++;
		RenderOrderNode node;
		node.id = new_id;
		node.parent = parent;

		m_order_nodes.emplace(new_id, node);		
		m_source_to_node[sourceId] = new_id;
		m_hierarchy_dirty = true;
		return new_id;
	}

	void RenderContext::unregisterOrderNode(RenderNodeId nodeId)
	{
		if (nodeId == InvalidRenderNode)
			return;

		auto it = m_order_nodes.find(nodeId);
		if (it == m_order_nodes.end())
			return;

		RenderOrderNode& node = it->second;

		// remove from parent's children
		if (node.parent != InvalidRenderNode) {
			auto parentIt = m_order_nodes.find(node.parent);
			if (parentIt != m_order_nodes.end()) {
				auto& siblings = parentIt->second.children;
				siblings.erase(std::remove(siblings.begin(), siblings.end(), nodeId), siblings.end());
			}
		}

		// remove from root nodes if necessary
		if (node.parent == InvalidRenderNode) {
			m_root_nodes.erase(std::remove(m_root_nodes.begin(), m_root_nodes.end(), nodeId), m_root_nodes.end());
		}

		m_order_nodes.erase(it);
		
		// remove from source_to_node map
		for (auto mapIt = m_source_to_node.begin(); mapIt != m_source_to_node.end(); ) {
			if (mapIt->second == nodeId)
				mapIt = m_source_to_node.erase(mapIt);
			else
				++mapIt;
		}
	}

	RenderNodeId RenderContext::findNodeForSource(GraphicsNodeId sourceId) const
	{
		// find the render node associated with the given graphics node ID
		auto it = m_source_to_node.find(sourceId);
		if (it != m_source_to_node.end())
			return it->second;
		return InvalidRenderNode;
	}

	void RenderContext::attachRenderable(RenderNodeId nodeId, RenderableHandle renderable)
	{
		if (nodeId == InvalidRenderNode || renderable == InvalidRenderable)
			return;

		auto it = m_order_nodes.find(nodeId);
		if (it == m_order_nodes.end())
			return;

		RenderOrderNode& node = it->second;

		// remove old insertion-order entry if this node already had a different renderable
		if (node.renderable != InvalidRenderable && node.renderable != renderable) {
			auto oldIt = std::find(m_ordered_renderables.begin(), m_ordered_renderables.end(), node.renderable);
			if (oldIt != m_ordered_renderables.end())
				m_ordered_renderables.erase(oldIt);
		}

		node.renderable = renderable;

		// insertion mode still needs a flat list
		if (std::find(m_ordered_renderables.begin(), m_ordered_renderables.end(), renderable) == m_ordered_renderables.end())
			m_ordered_renderables.push_back(renderable);

		m_hierarchy_dirty = true;
	}

	void RenderContext::detachRenderable(RenderNodeId nodeId)
	{
		if (nodeId == InvalidRenderNode)
			return;

		auto it = m_order_nodes.find(nodeId);
		if (it == m_order_nodes.end())
			return;

		RenderableHandle renderable = it->second.renderable;
		if (renderable != InvalidRenderable) {
			auto rendIt = std::find(m_ordered_renderables.begin(), m_ordered_renderables.end(), renderable);
			if (rendIt != m_ordered_renderables.end())
				m_ordered_renderables.erase(rendIt);
		}

		it->second.renderable = InvalidRenderable;

		m_hierarchy_dirty = true;
	}

	void RenderContext::setChildOrder(RenderNodeId parent_node_id, const std::vector<RenderNodeId>& ordered_children)
	{
		for (int i = 0; i < ordered_children.size(); ++i) {
			auto it = m_order_nodes.find(ordered_children[i]);
			if (it != m_order_nodes.end())
				it->second.sort_index = i;
		}

		m_hierarchy_dirty = true;		
	}

	std::vector<RenderableHandle> RenderContext::debugCollectOrder(RenderWorld* world)
	{
		std::vector<RenderableHandle> result;
		for (auto root_id : m_root_nodes) {
			debugCollectOrderRecursive(root_id, result, world);
		}
		return result;
	}

	// Helper function for debugCollectOrder to recursively collect renderables
	void RenderContext::debugCollectOrderRecursive(RenderNodeId node_id, std::vector<RenderableHandle>& result, RenderWorld* world)
	{
		auto it = m_order_nodes.find(node_id);
		if (it == m_order_nodes.end())
			return;

		const auto& node = it->second;

		if (!node.visible)
			return;

		if (node.renderable != InvalidRenderable)
			result.push_back(node.renderable);

		for (RenderNodeId child : node.children)
			debugCollectOrderRecursive(child, result, world);
	}

	bool RenderContext::validateTree()
	{
		std::unordered_set<RenderNodeId> visited;
		for (auto root_id : m_root_nodes) {
			if (!validateTreeRecursive(root_id, visited))
				return false;
		}
		return true;
	}

	// helper function for recursive tree validation
	bool RenderContext::validateTreeRecursive(RenderNodeId node_id, std::unordered_set<RenderNodeId>& visited)
	{
		// cycle detection
		if (visited.find(node_id) != visited.end())
			return false;
		visited.insert(node_id);

		auto it = m_order_nodes.find(node_id);
		if (it == m_order_nodes.end())
			return false;

		const auto& node = it->second;

		// check if all child nodes are valid
		for (RenderNodeId child : node.children)
		{
			if (!validateTreeRecursive(child, visited))
				return false;
		}
		return true;
	}
}
