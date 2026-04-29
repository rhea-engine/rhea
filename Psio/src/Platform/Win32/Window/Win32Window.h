/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Windows.h>
#include <Psio/Psio.h>
#include <Psio/Window/Window.h>

namespace psio
{
	class Win32Window : public psio::Window
	{
	public:
		Win32Window(WindowDesc desc) :
			m_name(desc.name),
			m_close(false),
			m_HWnd(nullptr),
			m_HInstance(nullptr),
			m_wp{},
			desc(desc),
			Window(desc.width, desc.height)
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
		WindowDesc desc;
	};
}