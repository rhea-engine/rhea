#ifndef HIPPIOS_D3D11_SWAPCHAIN_H
#define HIPPIOS_D3D11_SWAPCHAIN_H

#include <d3d11.h>
#include <Hippios/SwapChain.h>

namespace Hippios
{
	class D3D11SwapChain : public SwapChain
	{
	public:
		D3D11SwapChain(IDXGISwapChain* swap_chain) :
			m_swapchain(swap_chain)
		{}
		~D3D11SwapChain();

		bool isValid() const override;

		// methods from base class
		void onResize(int width, int height);

		void SetFullscreen(bool mode);

		// get the native swapchain
		IDXGISwapChain* getNativeSwapChain() { return m_swapchain; }
	protected:
		IDXGISwapChain* m_swapchain;
	};
}

#endif // !HIPPIOS_D3D11_SWAPCHAIN_H
