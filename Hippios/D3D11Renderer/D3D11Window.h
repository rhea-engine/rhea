#ifndef HIPPIOS_D3D11WINDOW_H
#define HIPPIOS_D3D11WINDOW_H

#include <d3d11.h>

#include "Hippios/Window.h"
#include "ComPtr.h"
#include "Components/Psio/Window.h"
#include "Hippios/RenderListener.h"

namespace Hippios
{
	class Viewport;

	class D3D11Window : public HippiosWindow
	{
	protected:
		Psio::Window* mWin = 0;		
		Viewport* m_viewport = 0;
		RenderListener* m_scene = 0;

		// the swap chain
		ComPtr<IDXGISwapChain>      mDXGISwapChain;

		// DXGI swap chain device resources
		ComPtr < ID3D11Texture2D>        mBackBuffer;
		ComPtr < ID3D11RenderTargetView> mRenderTarget;

	public:
		D3D11Window();
		/*HRESULT CreateSwapchain(HWND hWnd);
		HRESULT ConfigureBackBuffer();*/
		void create(Psio::Engine* psio);
		void addViewport(Viewport* viewport);
		void update();
		void destroy();	
		Psio::Window* getWindow() {
			return mWin;
		}
	};
}
#endif // !HIPPIOS_D3D11WINDOW_H