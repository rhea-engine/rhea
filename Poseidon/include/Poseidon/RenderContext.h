#ifndef POSEIDON_RENDERCONTEXT_H
#define POSEIDON_RENDERCONTEXT_H

#include <string>
#include <vector>

#include "Node.h"
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Light.h>
#include <Hippios/BaseTypes.h>

using Rhea::Item;

namespace Poseidon
{
	class Camera;
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
		RenderContext(std::string& name);

		// name handling
		void setName(std::string& name) { m_name = name; }
		std::string getName() { return m_name; }
		bool isName(std::string& name) { return m_name == name; }

		// get lights
		void getLights(std::vector<Rhea::Light*>& lights);

		// cull the visible objects
		void cullObjects(RenderQueue* queue);

		// add render child
		void addRenderChild(Item* child);

		// get all render children
		std::vector<Item*>& getRenderChildren() { return m_render_children; }

		// remove render element
		void removeRenderChild(Item* child);

		Camera* get_camera() { return m_camera; };
		void setCamera(Camera* camera);

		// stencil state of the context
		void setStencilState(int state_id) { m_stencil_state = state_id; }
		int getStencilState() { return m_stencil_state; }

		void setSize(float width, float height) { m_size.w = width; m_size.h = height; }
		Hippios::ViewportDesc* getSize() { return &m_size; }

		// ambient light
		void setAmbient(Math::Quaternion ambient) { m_ambient = ambient; }
		Math::Quaternion getAmbient() { return m_ambient; }

	protected:
		std::string m_name;
		Camera* m_camera;
		std::vector<Item*> m_render_children;
		int m_stencil_state;
		Math::Quaternion m_ambient;
		Hippios::ViewportDesc m_size;
	};
}

#endif // !POSEIDON_RENDERCONTEXT_H
