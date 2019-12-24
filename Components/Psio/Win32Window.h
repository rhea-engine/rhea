#include <Windows.h>
#include "Window.h"

namespace Psio
{
	class Win32Window : public Psio::Window
	{
	protected:
		int mClose = false;
		HWND mHWnd = 0;
		HINSTANCE mHInstance = 0;
	public:
		void create();
		void destroy();
		virtual void* getHandle() {
			return (void*)mHWnd;
		}
		HINSTANCE get_instance() { return mHInstance; };
	};
}