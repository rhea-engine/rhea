#ifndef PSIO_H
#define PSIO_H

#include <memory>

namespace Psio
{
	class Input;
	class InputEvent;
	class Window;

	class Engine
	{
	public:
		Engine();

		// create a window
		Window* createWindow();

		// get input Handler
		Input* getInput() { return m_input; }

		// get the next input event
		std::shared_ptr<InputEvent> pollInput();

	protected:
		Input* m_input;
	};
}
#endif // !PSIO_H
