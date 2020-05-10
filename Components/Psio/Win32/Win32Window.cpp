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

		switch (uMsg)
		{
		case WM_CLOSE:
		{
			win->destroy();
			return 0;
		}

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
			std::static_pointer_cast<KeyInputEvent>(evt)->set(hwKey(wParam), scancode, 0);
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
			shift = GetKeyState(VK_SHIFT) & 0x0800;
			alt = GetKeyState(VK_MENU) & 0x0800;
			ctrl = GetKeyState(VK_CONTROL) & 0x0800;
			evt = std::make_shared<MouseInputEvent>(Psio::InputEventId::MOUSE_BTN_PRESSED);
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

	void Win32Window::create()
	{
		UINT classStyle = 0;
		WNDPROC windowProc = StaticWindowProc;
		HINSTANCE hInst = NULL;
		HWND parentHWnd = NULL;
		DWORD dwStyle = 0;

		// set inital window size
		mWidth = 1152;
		mHeight = 864;

		// some default defines
		classStyle |= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS; // TODO: make configurable?  , CS_HREDRAW | CS_VREDRAW means redraw on v or h move of the win (not needed for fs)
		mHInstance = (HINSTANCE)GetModuleHandle(NULL);

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

		// compute win size
		RECT wr = { 0, 0, mWidth, mHeight };    // set the size, but not the position
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);    // adjust the size


		// Create the window for our viewport.
		mHWnd = CreateWindowEx(dwStyle, PSIO_WIN_CLASS_NAME, "Poseidon", WS_OVERLAPPEDWINDOW, 300, 300, wr.right - wr.left, wr.bottom - wr.top,
			parentHWnd,
			NULL,
			mHInstance,
			this);
		if (mHWnd == NULL)
		{
			MessageBox(NULL, "Window Creation Failed!", "Error!",
				MB_ICONEXCLAMATION | MB_OK);
			return;
		}

		// show the window
		ShowWindow(mHWnd, SW_SHOWNORMAL);
		UpdateWindow(mHWnd);
	}

	void Win32Window::destroy()
	{
		HMENU hMenu;
		hMenu = GetMenu(mHWnd);
		if (hMenu != NULL)
		{
			DestroyMenu(hMenu);
		}
		DestroyWindow(mHWnd);
		UnregisterClass(
			PSIO_WIN_CLASS_NAME,
			mHInstance
		);
	}
}