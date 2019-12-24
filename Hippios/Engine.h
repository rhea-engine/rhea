#ifndef HIPPIOS_ENGINE_H
#define HIPPIOS_ENGINE_H

namespace Hippios
{
	class Renderer;
	class HippiosWindow;

	class Engine
	{
	protected:
		Renderer* m_renderer = 0;
		HippiosWindow* m_window = 0;
	public:
		void startup();		
		Renderer* getRenderer() { return m_renderer; };
		void setRenderer(Renderer* renderer);
		HippiosWindow* createWindow();
		bool render();
		HippiosWindow* get_window() {
			return m_window;
		}
	};
}
#endif // !HIPPIOS_ENGINE_H
