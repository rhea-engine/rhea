#ifndef POSEIDON_RENDERCONTEXT_H
#define POSEIDON_RENDERCONTEXT_H

#include <string>
#include <vector>

#include "Node.h"

namespace Poseidon
{
	class Camera;
	class Renderable;
	class RenderQueue;

	/** a class that holds a render context.
	*   a render context has its render queue and its own camera
	*   render contexts belong to onw viewport and are rendered in the sequence in which they are added to the vieport
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

		// cull the visible objects
		void cullObjects(RenderQueue* queue);

		// add render child
		void addRenderChild(Renderable* child);

		// remove render element
		void removeRenderChild(Renderable* child);

		Camera* get_camera() { return m_camera; };
		void setCamera(Camera* camera);

	protected:
		std::string m_name;
		Camera* m_camera;
		std::vector<Renderable*> m_render_children;
	};
}

#endif // !POSEIDON_RENDERCONTEXT_H
