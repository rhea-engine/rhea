#include "D3D11RenderTarget.h"

#include <d3d11.h>
#include <D3D11SwapChain.h>
#include <D3D11Renderer.h>

namespace Hippios
{
	D3D11RenderTarget::~D3D11RenderTarget()
	{
		if (m_backbuffer)
			m_backbuffer->Release();
	}

	bool D3D11RenderTarget::create(Renderer* renderer, int width, int height, SwapChain* swap_chain)
	{		
		m_renderer = renderer;
		return create(width, height, swap_chain);
	}

	bool D3D11RenderTarget::create(int width, int height, SwapChain* swap_chain)
	{
		IDXGISwapChain* d3_swap_chain = static_cast<D3D11SwapChain*>(swap_chain)->getNativeSwapChain();
		ID3D11Device* d3_device = static_cast<D3D11Renderer*>(m_renderer)->get_device();
		ID3D11DeviceContext* d3_device_context = static_cast<D3D11Renderer*>(m_renderer)->get_device_context();

		Release();

		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer;
		d3_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (!pBackBuffer)
			return false;

		// use the back buffer address to create the render target	
		d3_device->CreateRenderTargetView(pBackBuffer, NULL, &m_backbuffer);
		pBackBuffer->Release();

		// create the depth buffer
		m_depth_buffer = std::make_unique<D3D11DepthBuffer>(d3_device, 24, width, height);

		return true;
	}

	void D3D11RenderTarget::Release()
	{
		if (m_backbuffer) {
			m_backbuffer->Release();
			m_backbuffer = nullptr;
		}
		if (m_depth_buffer)
			m_depth_buffer.reset();
	}

	void D3D11RenderTarget::setActive()
	{
		ID3D11DeviceContext* d3_device_context = static_cast<D3D11Renderer*>(m_renderer)->get_device_context();

		// set the render target as the back buffer
		d3_device_context->OMSetRenderTargets(1, &m_backbuffer, m_depth_buffer->get_view());
	}

	void D3D11RenderTarget::clearPass()
	{
		ID3D11DeviceContext* d3_device_context = static_cast<D3D11Renderer*>(m_renderer)->get_device_context();

		// clear the back buffer to a deep blue
		float color[4] = { 0.0f, 0.0f, 0.5f, 0.0f };
		d3_device_context->ClearRenderTargetView(m_backbuffer, color);

		//Refresh the Depth/Stencil view
		d3_device_context->ClearDepthStencilView(m_depth_buffer->get_view(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void D3D11RenderTarget::setViewport(ViewportDesc* desc)
	{
		ID3D11DeviceContext* d3_device_context = static_cast<D3D11Renderer*>(m_renderer)->get_device_context();

		// Set the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = desc->x;
		viewport.TopLeftY = desc->y;
		viewport.Width = desc->w;
		viewport.Height = desc->h;
		viewport.MinDepth = desc->min_depth;
		viewport.MaxDepth = desc->max_depth;

		d3_device_context->RSSetViewports(1, &viewport);
	}
}