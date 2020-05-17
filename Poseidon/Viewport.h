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

	class Viewport : public Node
	{
	public: 		
		Viewport();

		// cull the fisible objects
		void cullObjects(RenderQueue* queue);

		virtual void initRenderTarget(Hippios::Engine* engine);
		Hippios::Engine* get_renderer() { return m_engine; };
		void addRenderChild(Renderable* child);
		
		// remove render element
		void removeRenderChild(Renderable* child);

		Camera* get_camera() { return m_camera; };
		void setCamera(Camera* camera);

	protected:
		Camera* m_camera = 0; // the active camera
		Hippios::Engine* m_engine = 0; // reference to the render system
		Hippios::Renderer* m_renderer = 0;
		std::vector<Renderable*> m_render_children;
		Hippios::HippiosWindow* m_win = 0;

		virtual void registerNode();
		virtual void registerGroup(std::string group_name, Node* node);
	};

}

#endif // !POSEIDON_VIEWPORT_H