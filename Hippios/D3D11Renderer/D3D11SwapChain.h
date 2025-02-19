#ifndef HIPPIOS_D3D11_SWAPCHAIN_H
#define HIPPIOS_D3D11_SWAPCHAIN_H

#include <d3d11.h>
#include "../SwapChain.h"

namespace Hippios
{
	class D3D11SwapChain : public SwapChain
	{
	public:
		D3D11SwapChain(IDXGISwapChain* swap_chain) :
			m_swapchain(swap_chain)
		{}
		~D3D11SwapChain();

		// methods from base class
		void onResize(int width, int height);

		// get the native swapchain
		IDXGISwapChain* getNativeSwapChain() { return m_swapchain; }
	protected:
		IDXGISwapChain* m_swapchain;
	};
}

#endif // !HIPPIOS_D3D11_SWAPCHAIN_H
