#include <Windows.h>
#include "../Window.h"

namespace Psio
{
	class Win32Window : public Psio::Window
	{
	public:
		Win32Window(std::string name, int width, int height) :
			m_name(name),
			m_close(false),
			m_HWnd(nullptr),
			m_HInstance(nullptr),

			Window(width, height)
		{}

		void SetFullscreen(bool mode);

		void create() override;
		void destroy() override;
		void* getHandle() override {
			return (void*)m_HWnd;
		}
		HINSTANCE get_instance() { return m_HInstance; };

	protected:
		int m_close = false;
		HWND m_HWnd = 0;
		HINSTANCE m_HInstance = 0;
		WINDOWPLACEMENT m_wp;
		std::string m_name;
	};
}