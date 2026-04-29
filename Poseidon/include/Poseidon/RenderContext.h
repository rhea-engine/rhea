/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef POSEIDON_RENDERCONTEXT_H
#define POSEIDON_RENDERCONTEXT_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <Math/Quaternion.h>
#include <Hippios/BaseTypes.h>
#include "RenderTypes.h"

namespace poseidon
{
//	class Renderable;
	class RenderQueue;
	class RenderWorld;

	using RenderNodeId = uint64_t;
	constexpr RenderNodeId InvalidRenderNode = 0;

	using GraphicsNodeId = uint64_t;
	constexpr GraphicsNodeId InvalidGraphicsNode = 0;

	/** a class that holds a render context.
	*   a render context has its render queue and its own camera
	*   render contexts belong to one viewport and are rendered in the sequence in which they are added to the viewport
	*/
	class RenderContext
	{
	public:
		RenderContext() :
			RenderContext(std::string("root"))
		{}
		RenderContext(std::string name);

		enum class RenderOrderMode {
			Insertion,   // sort by insertion order
			TreeOrdered     // sort by tree order
		};

		struct RenderOrderNode
		{
			RenderNodeId id = InvalidRenderNode;
			RenderNodeId parent = InvalidRenderNode;
			std::vector<RenderNodeId> children;
			bool visible = true;
			int sort_index = 0;

			RenderableHandle renderable = InvalidRenderable;
		};		

		// name handling
		void setName(std::string& name) { m_name = name; }
		std::string getName() { return m_name; }
		bool isName(std::string& name) { return m_name == name; }

		// get lights
		const std::vector<LightHandle>& getLights() const { return m_lights; }

		// add a light
		void addLight(LightHandle handle);
		// remove a light
		void removeLight(LightHandle handle);

		// cull the visible objects
		void cullObjects(RenderQueue* queue, RenderWorld* world);
		void collectOrderedRecursive(RenderNodeId node_id, RenderQueue* queue, RenderWorld* world);		

		RenderNodeId registerOrderNode(GraphicsNodeId sourceId, RenderNodeId parent);
		void unregisterOrderNode(RenderNodeId nodeId);

		RenderNodeId findNodeForSource(GraphicsNodeId sourceId) const;

		void attachRenderable(RenderNodeId nodeId, RenderableHandle renderable);
		void detachRenderable(RenderNodeId nodeId);

		void rebuildHierarchy();

		void setChildOrder(RenderNodeId parent_node_id, const std::vector<RenderNodeId>& ordered_children);
		void setVisibility(RenderNodeId nodeId, bool mode);

		CameraHandle getCameraHandle() const { return m_camera_handle; }
		void setCameraHandle(CameraHandle handle) { m_camera_handle = handle; }

		// stencil state of the context
		void setStencilState(int state_id) { m_stencil_state = state_id; }
		int getStencilState() { return m_stencil_state; }

		void setSize(float width, float height) { m_size.w = width; m_size.h = height; }
		hippios::ViewportDesc* getSize() { return &m_size; }

		// ambient light
		void setAmbient(math::Quaternion ambient) { m_ambient = ambient; }
		math::Quaternion getAmbient() { return m_ambient; }

		// render order mode
		void setRenderOrderMode(RenderOrderMode mode) { m_render_order_mode = mode; }
		RenderOrderMode getRenderOrderMode() { return m_render_order_mode; }	

		const std::vector<RenderableHandle>& getRenderables() const { return m_ordered_renderables; }
	
		// debugging
		std::vector<RenderableHandle> debugCollectOrder(RenderWorld* world);
		void debugCollectOrderRecursive(RenderNodeId node_id, std::vector<RenderableHandle>& result, RenderWorld* world);
		bool validateTree();
		bool validateTreeRecursive(RenderNodeId node_id, std::unordered_set<RenderNodeId>& visited);

	private:	
		void sortNodes(std::vector<RenderNodeId>& nodes);

	protected:
		std::string m_name;
		CameraHandle m_camera_handle = InvalidCamera;
		
		std::vector<LightHandle> m_lights;
		int m_stencil_state;
		math::Quaternion m_ambient;
		hippios::ViewportDesc m_size;
		RenderOrderMode m_render_order_mode = RenderOrderMode::Insertion;
		

		bool m_hierarchy_dirty = false;

		std::unordered_map<GraphicsNodeId, RenderNodeId> m_source_to_node;

		std::unordered_map<RenderNodeId, RenderOrderNode> m_order_nodes;
		std::vector<RenderNodeId> m_root_nodes;
		std::vector<RenderableHandle> m_ordered_renderables;

		RenderNodeId m_next_node_id = 1;
	};
}

#endif // !POSEIDON_RENDERCONTEXT_H
