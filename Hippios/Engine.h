#ifndef HIPPIOS_ENGINE_H
#define HIPPIOS_ENGINE_H

#include <memory>
#include "Renderer.h"

namespace Psio
{
	class Engine;
}

namespace Hippios
{
	class HippiosWindow;
	class VertexBuffer;
	class IndexBuffer;

	class Engine
	{
	public:
		Engine(Psio::Engine* psio) :
			m_psio(psio),
			m_main_window(nullptr)		
		{}

		void startup();		
		Renderer* getRenderer() { return m_renderer.get(); };
		void setRenderer(std::unique_ptr<Renderer> renderer);
		
		// create a new window
		HippiosWindow* createWindow();
		
		bool render();
		
		// return the main window
		HippiosWindow* getMainWindow() { return m_main_window; }
		// set the main window
		void setMainWindow(HippiosWindow* window) { m_main_window = window; }
		
		// bind to vertex and index buffer
		void update(VertexBuffer* vb, IndexBuffer* ib);

	protected:
		std::unique_ptr<Renderer> m_renderer;
		HippiosWindow* m_main_window ;
		Psio::Engine* m_psio;
	};
}
#endif // !HIPPIOS_ENGINE_H
