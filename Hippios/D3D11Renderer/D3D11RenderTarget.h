#ifndef HIPPIOS_D3D11_RENDERTARGET_H
#define HIPPIOS_D3D11_RENDERTARGET_H

#include "D3D11Renderer.h"
#include <Hippios/RenderTarget.h>

#include <d3d11.h>

namespace Hippios
{
	class D3D11RenderTarget : public RenderTarget
	{
	public:
		~D3D11RenderTarget();
			
		bool resize(int width, int height);

		bool create(Renderer* renderer, const RenderTargetDesc& desc) override;

		void Release() override;

		void setActive() override;
		void clearPass() override;
		void setViewport(ViewportDesc* desc) override;

	protected:
		D3D11Renderer* m_renderer = nullptr;	// reference to renderer
		ID3D11RenderTargetView* m_backbuffer = nullptr;    // the pointer to our back buffer
		std::unique_ptr<D3D11DepthBuffer> m_depth_buffer;
		int m_width = 0;
		int m_height = 0;
		Hippios::SwapChain* m_swap_chain = nullptr;
	};
}

#endif // !HIPPIOS_D3D11_RENDERTARGET_H
