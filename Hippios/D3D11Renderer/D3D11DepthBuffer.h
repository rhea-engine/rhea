#ifndef HIPPIOS_D3D11DEPTHBUFFER_H
#define HIPPIOS_D3D11DEPTHBUFFER_H

#include <d3d11.h>

#include <Hippios/DepthBuffer.h>

namespace hippios
{
	class D3D11DepthBuffer : public DepthBuffer
	{	
	public:
		D3D11DepthBuffer(ID3D11Device* d3d_device, unsigned int bit_depth, unsigned int width, unsigned int height);
		~D3D11DepthBuffer();
		ID3D11DepthStencilView* get_view() { return m_depth_stencil_view; };
	protected:
		ID3D11Device* m_device;
		ID3D11DepthStencilView* m_depth_stencil_view;
		ID3D11Texture2D* m_depth_stencil_buffer;
	};
}

#endif // !HIPPIOS_D3D11DEPTHBUFFER_H
