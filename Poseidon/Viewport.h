#ifndef POSEIDON_VIEWPORT_H
#define POSEIDON_VIEWPORT_H

#include "Hippios/Engine.h"
#include "Hippios/Renderer.h"
#include "Hippios/Window.h"

#include "Camera.h"
#include "Node.h"

namespace Poseidon
{
	class Animatable;
	class Entity;

	class Viewport : public Node
	{
	public: 		
		Viewport();

		bool render();
		virtual void initRenderTarget(Hippios::Engine* engine);
		Hippios::Engine* get_renderer() { return m_engine; };
		void addRenderChild(Animatable* child);
		Camera* get_camera() { return m_camera; };
		void setCamera(Camera* camera);

	protected:
		Camera* m_camera = 0; // the active camera
		Hippios::Engine* m_engine = 0; // reference to the render system
		Hippios::Renderer* m_renderer = 0;
		std::vector<Animatable*> m_render_children;
		Hippios::HippiosWindow* m_win = 0;
	};

}

#endif // !POSEIDON_VIEWPORT_H