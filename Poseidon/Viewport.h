#ifndef POSEIDON_VIEWPORT_H
#define POSEIDON_VIEWPORT_H

#include "Hippios/Engine.h"
#include "Hippios/Renderer.h"
#include "Hippios/Window.h"

#include "Camera.h"

namespace Poseidon
{
	class Mesh;

	class Viewport
	{
		Camera* m_camera = 0; // the active camera
		Hippios::Engine* m_engine = 0; // reference to the render system
		Hippios::Renderer* m_renderer = 0;
		std::vector<Mesh*> m_render_children;
		Hippios::HippiosWindow* m_win = 0;
	public: 		
		bool onUpdate();
		void initRenderTarget(Hippios::Engine* engine);
		Hippios::Engine* get_renderer() { return m_engine; };
		void addRenderChild(Mesh* mesh);
		Camera* get_camera() { return m_camera; };
		void setCamera(Camera* camera);
	};

}

#endif // !POSEIDON_VIEWPORT_H