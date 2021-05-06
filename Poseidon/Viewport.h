#ifndef POSEIDON_VIEWPORT_H
#define POSEIDON_VIEWPORT_H

#include "Hippios/Engine.h"
#include "Hippios/Renderer.h"
#include "Hippios/Window.h"

#include "Camera.h"
#include "Node.h"

namespace Poseidon
{
	class Renderable;
	class Entity;
	class RenderQueue;
	class Camera;
	class RenderContext;

	class Viewport : public Node
	{
	public: 		
		Viewport() :
			Viewport(std::string("root"))
		{}
		Viewport(std::string& name);
		~Viewport();

		virtual void initRenderTarget(Hippios::Engine* engine);
		Hippios::Engine* get_renderer() { return m_engine; };

		// render contexts
		RenderContext* getRenderContext(std::string& name);
		RenderContext* createRenderContext(std::string& name);
		std::vector<RenderContext*> getRenderContextList() { return m_render_contexts; }

		// name handling
		void setName(std::string& name) { m_name = name; }
		std::string getName() { return m_name; }
		bool isName(std::string& name) { return m_name == name; }

		// get height of viewport
		int getHeight();
		// get width of viewport
		int getWidth();

	protected:
		std::string m_name;
		Camera* m_camera = 0; // the active camera
		Hippios::Engine* m_engine = 0; // reference to the render system
		Hippios::Renderer* m_renderer = 0;
		Hippios::HippiosWindow* m_win = 0;

		std::vector<RenderContext*> m_render_contexts;

		virtual void registerNode();
		virtual void registerGroup(std::string group_name, Node* node);
	};

}

#endif // !POSEIDON_VIEWPORT_H