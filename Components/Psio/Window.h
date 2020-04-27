#ifndef PSIO_WINDOW_H
#define PSIO_WINDOW_H

#include <memory>

namespace Psio
{
	class Input;
	class InputEvent;

	class Window
	{
	protected:
		int mWidth;
		int mHeight;
		Input* m_input;
	public:
		virtual void create() = 0;
		virtual void destroy() = 0;
		static Window* createWindow();
		virtual void* getHandle() = 0;
		int GetWidth() { return mWidth; }
		int GetHeight() { return mHeight; }

		// set input queue
		void setInput(Input* input);

		// add to input queue
		void addInputEvent(std::shared_ptr<InputEvent> event);

		void onKeyDown(unsigned int key);
	};
}
#endif // !PSIO_WINDOW_H