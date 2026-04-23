#ifndef POSEIDON_RENDERCONTEXT_H
#define POSEIDON_RENDERCONTEXT_H

#include <string>
#include <vector>

#include "Node.h"
#include <Rhea/Core/Item.h>
#include <Hippios/BaseTypes.h>
#include "RenderTypes.h"

using rhea::Item;

namespace poseidon
{
//	class Renderable;
	class RenderQueue;

	/** a class that holds a render context.
	*   a render context has its render queue and its own camera
	*   render contexts belong to one viewport and are rendered in the sequence in which they are added to the viewport
	*/
	class RenderContext : public Node
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
		void cullObjects(RenderQueue* queue);

		void cullOrderedRecursive(Item* item, RenderQueue* queue);

		// add render child
		void addRenderChild(Item* child);
		void AddRenderable(RenderableHandle handle);

		// check if render child is already added
		bool containsRenderChild(Item* item) const;

		// check if item is a root in this context
		bool isRootInContext(Item* item) const;

		// get all render children
		std::vector<Item*>& getRenderChildren() { return m_render_children; }

		// remove render element
		void removeRenderChild(Item* child);
		void RemoveRenderable(RenderableHandle handle);		

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

		void rebuildRoots();

	protected:
		std::string m_name;
		CameraHandle m_camera_handle = InvalidCamera;
		std::vector<Item*> m_render_children;
		std::vector<RenderableHandle> m_renderables;
		std::vector<LightHandle> m_lights;
		int m_stencil_state;
		math::Quaternion m_ambient;
		hippios::ViewportDesc m_size;
		RenderOrderMode m_render_order_mode = RenderOrderMode::Insertion;
		std::vector<Item*> m_render_roots;
	};
}

#endif // !POSEIDON_RENDERCONTEXT_H
