#include "D3D11SwapChain.h"

namespace hippios
{
	D3D11SwapChain::~D3D11SwapChain()
	{
		if (m_swapchain) {
			m_swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode to allow D3D11 to close

			m_swapchain->Release();
		}
	}

	bool D3D11SwapChain::isValid() const
	{
		return (m_swapchain != nullptr);
	}

	void D3D11SwapChain::onResize(int width, int height)
	{
		if (m_swapchain)
			m_swapchain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		m_width = width;
		m_height = height;
	}

	void D3D11SwapChain::SetFullscreen(bool mode)
	{
		m_swapchain->SetFullscreenState(mode, nullptr);
	}
}