#ifndef HIPPIOS_ENGINE_H
#define HIPPIOS_ENGINE_H

namespace Psio
{
	class Engine;
}

namespace Hippios
{
	class Renderer;
	class HippiosWindow;
	class VertexBuffer;
	class IndexBuffer;

	class Engine
	{
	public:
		Engine(Psio::Engine* psio) :
			m_psio(psio)
		{}

		void startup();		
		Renderer* getRenderer() { return m_renderer; };
		void setRenderer(Renderer* renderer);
		
		// create a new window
		HippiosWindow* createWindow();
		
		bool render();
		
		// return the main window
		HippiosWindow* get_window() {
			return m_window;
		}
		
		// bind to vertex and index buffer
		void update(VertexBuffer* vb, IndexBuffer* ib);

	protected:
		Renderer* m_renderer = 0;
		HippiosWindow* m_window = 0;
		Psio::Engine* m_psio;
	};
}
#endif // !HIPPIOS_ENGINE_H
