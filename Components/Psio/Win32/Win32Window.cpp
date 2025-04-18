#include <windows.h>
#include "Win32Window.h"
#include <stdio.h>
#include "../Keyboard.h"
#include "../InputEvent.h"
#include <D2d1.h>
#include <comutil.h>
#include <Windowsx.h>

#define PSIO_WIN_CLASS_NAME "PoseidonSIOWnd"

namespace Psio
{
	float scaleX = 1.0f;
	float scaleY = 1.0f;

	LRESULT CALLBACK StaticWindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
	)
	{
		if (uMsg == WM_CREATE)
		{   // Store pointer to Win32Window in user data area
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));

			// initialize dpi scaling
	/*		ID2D1Factory* pFactory;
			if (FAILED(D2D1CreateFactory(
				D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
			{
				return -1;  // Fail CreateWindowEx.
			}
			FLOAT dpiX, dpiY;
			pFactory->GetDesktopDpi(&dpiX, &dpiY);
			scaleX = dpiX / 96.0f;
			scaleY = dpiY / 96.0f;
			*/

			return 0;
		}

		// look up window instance
		// note: it is possible to get a WM_SIZE before WM_CREATE
		Win32Window* win = (Win32Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (!win)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		
		bool shift;
		bool alt;
		bool ctrl;
		std::shared_ptr<InputEvent> evt;
		uint32_t scancode;
		bool mleft;
		bool mright;
		bool mmiddle;
		unsigned int width;
		unsigned int height;

		switch (uMsg)
		{
		case WM_CLOSE:
			win->destroy();
			return 0;
		
		case WM_SIZE:
			width = LOWORD(lParam);
			height = HIWORD(lParam);
			evt = std::make_shared<WinSizeEvent>(Psio::InputEventId::WIN_SIZE, width, height);
			win->addInputEvent(evt);
			break;
	
		case WM_DESTROY:		
			PostQuitMessage(0);
			break;

		case WM_KEYDOWN:	
		case WM_KEYUP:
			shift = GetKeyState(VK_SHIFT) & 0x0800;
			alt = GetKeyState(VK_MENU) & 0x0800;
			ctrl = GetKeyState(VK_CONTROL) & 0x0800;
			//printf("WM_KEYDOWN: %u shift:%d\n", wParam, (shift) ? 1 : 0);

			// get scancode (see https://docs.microsoft.com/de-ch/windows/win32/inputdev/wm-keydown)
			scancode = lParam >> 16 & 0xFF;

			if (uMsg == WM_KEYDOWN)
				evt = std::make_shared<KeyInputEvent>(Psio::InputEventId::KEY_PRESSED);
			else
				evt = std::make_shared<KeyInputEvent>(Psio::InputEventId::KEY_RELEASED);
			evt->setModifiers(shift, alt, ctrl);
			std::static_pointer_cast<KeyInputEvent>(evt)->set(hwKey((int)wParam), scancode, 0);
			win->addInputEvent(evt);

			break;

		case WM_CHAR:
			shift = GetKeyState(VK_SHIFT) & 0x0800;
			alt = GetKeyState(VK_MENU) & 0x0800;
			ctrl = GetKeyState(VK_CONTROL) & 0x0800; 
			
			evt = std::make_shared<KeyInputEvent>(Psio::InputEventId::TEXT);
			evt->setModifiers(shift, alt, ctrl);
			std::static_pointer_cast<KeyInputEvent>(evt)->set(0, 0, (unsigned char)wParam);
			win->addInputEvent(evt);

			break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_MOUSEMOVE:
			shift = GetKeyState(VK_SHIFT) & 0x0800;
			alt = GetKeyState(VK_MENU) & 0x0800;
			ctrl = GetKeyState(VK_CONTROL) & 0x0800;
			switch (uMsg) {
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
				evt = std::make_shared<MouseInputEvent>(Psio::InputEventId::MOUSE_BTN_PRESSED);
				break;
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
				evt = std::make_shared<MouseInputEvent>(Psio::InputEventId::MOUSE_BTN_RELEASED);
				break;
			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
				evt = std::make_shared<MouseInputEvent>(Psio::InputEventId::MOUSE_BTN_DBLCLICK);
				break;
			case WM_MOUSEMOVE:
				evt = std::make_shared<MouseInputEvent>(Psio::InputEventId::MOUSE_MOVE);
				break;
			}
			evt->setModifiers(shift, alt, ctrl);

			mleft = (wParam & MK_LBUTTON);
			mright = (wParam & MK_RBUTTON);
			mmiddle = (wParam & MK_MBUTTON);

			std::static_pointer_cast<MouseInputEvent>(evt)->setBtn(mleft, mright, mmiddle);
			std::static_pointer_cast<MouseInputEvent>(evt)->setPos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			win->addInputEvent(evt);

			break;
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	void Win32Window::SetFullscreen(bool mode)
	{
		if (mode) {
			// Save current window state
			GetWindowPlacement(m_HWnd, &m_wp);

			// Get monitor size
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(MonitorFromWindow(m_HWnd, MONITOR_DEFAULTTOPRIMARY), &mi);

			// Set borderless fullscreen
			SetWindowLong(m_HWnd, GWL_STYLE, WS_POPUP);
			SetWindowPos(m_HWnd, nullptr, mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_FRAMECHANGED | SWP_NOOWNERZORDER);

			// Set window style and extended style (to show in Alt-Tab)
			SetWindowLong(m_HWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			SetWindowLong(m_HWnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST); // Ensure it appears in Alt-Tab

			// Bring window to the foreground
			ShowWindow(m_HWnd, SW_SHOW);
			SetForegroundWindow(m_HWnd);
			SetFocus(m_HWnd);
		}
		else {
			// Restore original window
			SetWindowLong(m_HWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
			SetWindowPlacement(m_HWnd, &m_wp);
			SetWindowPos(m_HWnd, nullptr, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_FRAMECHANGED);
		}
	}

	void Win32Window::create()
	{
		UINT classStyle = 0;
		WNDPROC windowProc = StaticWindowProc;
		HINSTANCE hInst = NULL;
		HWND parentHWnd = NULL;
		DWORD dwStyle = 0;

		// set inital window size if not set
		if (!m_width)
			m_width = 1152;
		if (!m_height)
			m_height = 864;

		// some default defines
		classStyle |= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // TODO: make configurable?  , CS_HREDRAW | CS_VREDRAW means redraw on v or h move of the win (not needed for fs)
		m_HInstance = (HINSTANCE)GetModuleHandle(NULL);

		// fill the wndclass
		WNDCLASSEX wc;
		// clear out the window class for use
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		
		// fill defaults
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = classStyle;
		wc.lpfnWndProc = windowProc;
		wc.hInstance = hInst;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		// dont show empty window wc.hbrBackground = (HBRUSH)COLOR_WINDOW; // (HBRUSH)GetStockObject(BLACK_BRUSH),
		wc.lpszClassName = PSIO_WIN_CLASS_NAME;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);

		/*
		wc = { classStyle, windowProc, 0, 0, hInst,
				LoadIcon(0, IDI_APPLICATION),
				LoadCursor(NULL, IDC_ARROW),
				(HBRUSH)GetStockObject(BLACK_BRUSH),
				0, PSIO_WIN_CLASS_NAME };
				*/
		if (!RegisterClassEx(&wc))
		{
			DWORD dwError = GetLastError();
			if (dwError != ERROR_CLASS_ALREADY_EXISTS)
				return;
		}

		// Get monitor where the mouse is
		POINT cursor;
		GetCursorPos(&cursor);
		HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);

		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(monitor, &mi);

		// Get monitor size
		int screen_width = mi.rcMonitor.right - mi.rcMonitor.left;
		int screen_height = mi.rcMonitor.bottom - mi.rcMonitor.top;
		
		// compute win size
		RECT wr = { 0, 0, m_width, m_height };    // set the size, but not the position
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size

		// Calculate centered position relative to the monitor
		int posX = mi.rcMonitor.left + (screen_width - (wr.right - wr.left)) / 2;
		int posY = mi.rcMonitor.top + (screen_height - (wr.bottom - wr.top)) / 2;

		// Create the window for our viewport.
		m_HWnd = CreateWindowEx(dwStyle, PSIO_WIN_CLASS_NAME, m_name.c_str(), WS_OVERLAPPEDWINDOW, posX, posY, wr.right - wr.left, wr.bottom - wr.top,
			parentHWnd,
			NULL,
			m_HInstance,
			this);
		if (m_HWnd == NULL)
		{
			MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		// show the window
		ShowWindow(m_HWnd, SW_SHOWNORMAL);
		UpdateWindow(m_HWnd);
	}

	void Win32Window::destroy()
	{
		HMENU hMenu;
		hMenu = GetMenu(m_HWnd);
		if (hMenu != NULL)
		{
			DestroyMenu(hMenu);
		}
		DestroyWindow(m_HWnd);
		UnregisterClass(
			PSIO_WIN_CLASS_NAME,
			m_HInstance
		);
	}
}