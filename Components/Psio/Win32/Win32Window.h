#include <Windows.h>
#include "../Window.h"

namespace Psio
{
	class Win32Window : public Psio::Window
	{
	protected:
		int mClose = false;
		HWND mHWnd = 0;
		HINSTANCE mHInstance = 0;
	public:
		Win32Window(int width, int height) :
			Window(width, height)
		{}

		void create() override;
		void destroy() override;
		void* getHandle() override {
			return (void*)mHWnd;
		}
		HINSTANCE get_instance() { return mHInstance; };
	};
}