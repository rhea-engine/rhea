#ifndef PSIO_WINDOW_H
#define PSIO_WINDOW_H

#include <memory>
#include <string>

namespace Psio
{
	class Input;
	class InputEvent;

	class Window
	{
	public:		
		Window(int width, int height) : 
			m_width(width),
			m_height(height),			
			m_input(nullptr)
		{}
		virtual void create() = 0;
		virtual void destroy() = 0;
		
		virtual void* getHandle() = 0;
		int GetWidth() { return m_width; }
		int GetHeight() { return m_height; }

		virtual void SetFullscreen(bool mode) = 0;

		// set input queue
		void setInput(Input* input);

		// add to input queue
		void addInputEvent(std::shared_ptr<InputEvent> event);

		void onKeyDown(unsigned int key);
	protected:
		int m_width;
		int m_height;
		Input* m_input;
	};
}
#endif // !PSIO_WINDOW_H