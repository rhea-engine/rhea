#include <d3d11.h>

#include "D3D11DepthBuffer.h"

namespace Hippios
{
	D3D11DepthBuffer::D3D11DepthBuffer(ID3D11Device* d3d_device, unsigned int bit_depth, unsigned int width, unsigned int height) :
		DepthBuffer(bit_depth, width, height),
		m_device(d3d_device)
	{
		//Describe our Depth/Stencil Buffer
		D3D11_TEXTURE2D_DESC depthStencilDesc;

		depthStencilDesc.Width = m_width;
		depthStencilDesc.Height = m_height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		m_device->CreateTexture2D(&depthStencilDesc, NULL, &m_depth_stencil_buffer);
		if (m_depth_stencil_buffer)
			m_device->CreateDepthStencilView(m_depth_stencil_buffer, NULL, &m_depth_stencil_view);
		else
			m_depth_stencil_view = 0;
	}
}