#include <windows.h>
#include "Win32Window.h"

#define PSIO_WIN_CLASS_NAME "PoseidonSIOWnd"

namespace Psio
{
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
		Win32Window* win = (Win32Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (!win)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		
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
			unsigned int code = 0;
			switch (wParam)
			{
			case VK_LEFT:

				// Process the LEFT ARROW key. 
				code = KeyCode::KEY_LEFT;
				break;

			case VK_RIGHT:
				code = KeyCode::KEY_RIGHT;
				// Process the RIGHT ARROW key. 
				
				break;

			case VK_UP:

				// Process the UP ARROW key. 
				code = KeyCode::KEY_UP;

				break;

			case VK_DOWN:

				// Process the DOWN ARROW key. 
				code = KeyCode::KEY_DOWN;

				break;

			case VK_HOME:

				// Process the HOME key. 

				break;

			case VK_END:

				// Process the END key. 

				break;

			case VK_INSERT:

				// Process the INS key. 

				break;

			case VK_DELETE:

				// Process the DEL key. 

				break;

			case VK_F2:

				// Process the F2 key. 

				break;


				// Process other non-character keystrokes. 

			default:
				break;
			} 
			if (0 && code)
				win->onKeyDown(code);
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