#ifndef HIPPIOS_D3D11_RENDERTARGET_H
#define HIPPIOS_D3D11_RENDERTARGET_H

#include "D3D11Renderer.h"
#include "../RenderTarget.h"

#include <d3d11.h>

namespace Hippios
{
	class D3D11RenderTarget : public RenderTarget
	{
	public:
		~D3D11RenderTarget();

		bool create(Renderer* renderer, int width, int height, SwapChain* swap_chain) override;
		
		bool create(int width, int height, SwapChain* swap_chain) override;

		void Release() override;

		void setActive() override;
		void clearPass() override;
		void setViewport(ViewportDesc* desc) override;

	protected:
		Renderer* m_renderer = nullptr;	// reference to renderer
		ID3D11RenderTargetView* m_backbuffer = nullptr;    // the pointer to our back buffer
		std::unique_ptr<D3D11DepthBuffer> m_depth_buffer;
	};
}

#endif // !HIPPIOS_D3D11_RENDERTARGET_H
