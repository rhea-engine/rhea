/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <windows.h>
#include "Win32Window.h"
#include <stdio.h>
#include <Psio/Input/Keyboard.h>
#include <Psio/Input/InputEvent.h>

#include <D2d1.h>
#include <comutil.h>
#include <Windowsx.h>
#include "../../../Input/InputEventBuilder.h"

#define PSIO_WIN_CLASS_NAME "PoseidonSIOWnd"

namespace psio
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
			return 0;
		}

		// look up window instance
		// note: it is possible to get a WM_SIZE before WM_CREATE
		auto* win = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (!win)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		
		std::shared_ptr<InputEvent> evt;

		switch (uMsg)
		{
		case WM_CLOSE:
			DestroyWindow(hWnd);
			return 0;

		case WM_SIZE:
		{
			evt = std::make_shared<WinSizeEvent>(
				InputEventId::WIN_SIZE,
				LOWORD(lParam),
				HIWORD(lParam),
				win);
			win->addInputEvent(evt);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			const bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
			const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

			const uint32_t scancode = (lParam >> 16) & 0xFF;
			const auto id = (uMsg == WM_KEYDOWN)
				? psio::InputEventId::KEY_PRESSED
				: psio::InputEventId::KEY_RELEASED;

			evt = BuildKeyEvent(
				id,
				static_cast<uint32_t>(hwKey((int)wParam)),
				scancode,
				0,
				shift,
				alt,
				ctrl);

			win->addInputEvent(evt);
			break;
		}
		case WM_CHAR:
		{
			const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			const bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
			const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

			evt = BuildTextEvent(
				std::string(1, static_cast<char>(wParam)),
				shift,
				alt,
				ctrl);
			win->addInputEvent(evt);

			break;
		}
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
		{
			const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			const bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
			const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

			InputEventId id = InputEventId::NONE;
			switch (uMsg) {
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
				id = InputEventId::MOUSE_BTN_PRESSED;
				break;
			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
				id = InputEventId::MOUSE_BTN_RELEASED;
				break;
			case WM_LBUTTONDBLCLK:
			case WM_RBUTTONDBLCLK:
			case WM_MBUTTONDBLCLK:
				id = InputEventId::MOUSE_BTN_DBLCLICK;
				break;
			case WM_MOUSEMOVE:
				id = InputEventId::MOUSE_MOVE;
				break;
			}
			evt = BuildMouseEvent(
				id,
				GET_X_LPARAM(lParam),
				GET_Y_LPARAM(lParam),
				(wParam & MK_LBUTTON) != 0,
				(wParam & MK_RBUTTON) != 0,
				(wParam & MK_MBUTTON) != 0,
				shift,
				alt,
				ctrl);
			win->addInputEvent(evt);

			break;
		}
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL:
		{
			const bool shift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			const bool alt = (GetKeyState(VK_MENU) & 0x8000) != 0;
			const bool ctrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
			
			int wheelX = 0;
			int wheelY = 0;
			if (uMsg == WM_MOUSEHWHEEL)
				wheelX = GET_WHEEL_DELTA_WPARAM(wParam);
			else
				wheelY = GET_WHEEL_DELTA_WPARAM(wParam);

			// convert screen coordinates to client coordinates
			POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ScreenToClient(hWnd, &pt);

			evt = BuildMouseWheelEvent(
				InputEventId::MOUSE_WHEEL,
				wheelY,
				wheelX,
				pt.x,
				pt.y,
				shift,
				alt,
				ctrl);
			win->addInputEvent(evt);
			break;
		}
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
		HWND parentHWnd = nullptr;
		DWORD dwStyle = 0;

		// set inital window size if not set
		if (!m_width)
			m_width = 1152;
		if (!m_height)
			m_height = 864;

		// some default defines
		classStyle |= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // TODO: make configurable?  , CS_HREDRAW | CS_VREDRAW means redraw on v or h move of the win (not needed for fs)
		m_HInstance = (HINSTANCE)GetModuleHandle(nullptr);

		// fill the wndclass
		WNDCLASSEX wc;
		// clear out the window class for use
		ZeroMemory(&wc, sizeof(WNDCLASSEX));
		
		// fill defaults
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = classStyle;
		wc.lpfnWndProc = windowProc;
		wc.hInstance = m_HInstance;
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
		if (!m_HWnd)
			return;

		HMENU hMenu = GetMenu(m_HWnd);
		if (hMenu != nullptr)
		{
			DestroyMenu(hMenu);
		}
		DestroyWindow(m_HWnd);
		m_HWnd = nullptr;
		if (m_HInstance)
		{
			UnregisterClass(
				PSIO_WIN_CLASS_NAME,
				m_HInstance
			);
		}
	}
}