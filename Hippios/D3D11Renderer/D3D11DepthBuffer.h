#ifndef HIPPIOS_D3D11DEPTHBUFFER_H
#define HIPPIOS_D3D11DEPTHBUFFER_H

#include <d3d11.h>

#include "Hippios/DepthBuffer.h"

namespace Hippios
{
	class D3D11DepthBuffer : public DepthBuffer
	{
	protected:
		ID3D11Device* m_device = 0;
		ID3D11DepthStencilView* m_depth_stencil_view = 0;
		ID3D11Texture2D* m_depth_stencil_buffer = 0;
	public:
		D3D11DepthBuffer(ID3D11Device* d3d_device, unsigned int bit_depth, unsigned int width, unsigned int height);
		ID3D11DepthStencilView* get_view() { return m_depth_stencil_view; };
	};
}

#endif // !HIPPIOS_D3D11DEPTHBUFFER_H
