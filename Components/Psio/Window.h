#pragma once
namespace Psio
{
	enum KeyCode : unsigned int {
		KEY_NONE,
		KEY_LEFT,
		KEY_RIGHT,
		KEY_UP,
		KEY_DOWN
	};

	class Window
	{
	protected:
		int mWidth;
		int mHeight;
	public:
		virtual void create() = 0;
		virtual void destroy() = 0;
		static Window* createWindow();
		virtual void* getHandle() = 0;
		int GetWidth() { return mWidth; }
		int GetHeight() { return mHeight; }
		void onKeyDown(unsigned int key);
	};
}