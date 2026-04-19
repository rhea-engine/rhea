#ifndef POSEIDON_VIEWPORT_H
#define POSEIDON_VIEWPORT_H

#include "Node.h"
#include <memory>
#include <vector>
#include <string>

namespace poseidon
{
	class Window;
	class RenderContext;
	class RenderTarget;

	/// <summary>
	/// Viewport is one render target, where the rendering occurs (backbuffers etc.)
	/// Not to confuse with a renderer viewport which is simply a rectangle of the the render target
	/// The Viewport has a list of render contexts.
	/// </summary>
	class Viewport : public Node
	{
	public: 		
		Viewport() :
			Viewport(std::string("root"))
		{}
		Viewport(std::string name);
		~Viewport();

		// render contexts
		RenderContext* getRenderContext(std::string& name);
		RenderContext* createRenderContext(std::string& name);
		std::vector<std::unique_ptr<RenderContext>>& getRenderContextList() { return m_render_contexts; }
		const std::vector<std::unique_ptr<RenderContext>>& getRenderContextList() const { return m_render_contexts; }

		void setWindow(Window* window) { m_window = window; }
		Window* getWindow() const { return m_window; }

		// name handling
		void setName(std::string& name) { m_name = name; }
		std::string getName() { return m_name; }
		bool isName(const std::string& name) const { return m_name == name; }

		// resize the viewport
		virtual void onResize(int width, int height);		

		void setRect(int x, int y, int width, int height);

		// get position of viewport
		int getX() { return m_x; }
		int getY() { return m_y; }

		// get width of viewport
		int getWidth() { return m_width; }
		// get height of viewport
		int getHeight() { return m_height; }

		
	protected:
		std::string m_name;
		Window* m_window; // the window this viewport is associated with

		int m_x;
		int m_y;
		int m_width;
		int m_height;

		std::vector<std::unique_ptr<RenderContext>> m_render_contexts;		

		virtual void registerNode();
		virtual void registerGroup(std::string group_name, Node* node);
	};

}

#endif // !POSEIDON_VIEWPORT_H