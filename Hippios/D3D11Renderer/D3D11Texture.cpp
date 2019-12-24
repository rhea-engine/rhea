#include <d3d11.h>

#include "D3D11Texture.h"

namespace Hippios
{
	D3D11Texture::D3D11Texture(Renderer* renderer)
	{
		m_renderer = (D3D11Renderer*)renderer;
	}

	void D3D11Texture::loadImpl(unsigned int width, unsigned int height, unsigned char* image_data)
	{
		if (!width || !height || !image_data)
		{
			return;
		}

		// Create texture
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		// create the texture
		HRESULT hr = m_renderer->get_device()->CreateTexture2D(&desc, NULL, &m_texture);

		if (!m_texture)
			return;

		// set final texture, generated values may differ from submitted
		m_texture->GetDesc(&desc);

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
		ZeroMemory(&srv_desc, sizeof(srv_desc));
		srv_desc.Format = desc.Format;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.MipLevels = desc.MipLevels;
		hr = m_renderer->get_device()->CreateShaderResourceView(m_texture, &srv_desc, &m_shader_resource_texture_view);

		unsigned int rowPitch = width * 4;
		//m_renderer->get_device_context()->UpdateSubresource(m_texture, 0, NULL, &(decodedTexture[0]), rowPitch, rowPitch * height);
		//UpdateSubresource((ID3D11Resource*)object_.ptr_, subResource, &destBox, data, rowSize, 0);
		D3D11_BOX destBox;
		destBox.left = (UINT)0;
		destBox.right = (UINT)(0 + width);
		destBox.top = (UINT)0;
		destBox.bottom = (UINT)(0 + height);
		destBox.front = 0;
		destBox.back = 1; 
		m_renderer->get_device_context()->UpdateSubresource(m_texture, 0, &destBox, image_data, rowPitch, 0);

		/*D3D11HardwarePixelBuffer* buffer;
		buffer = new D3D11HardwarePixelBuffer(
			this, // parentTexture
			mDevice, // device
			mip,
			width,
			height,
			depth,
			face,
			mFormat,
			(HardwareBuffer::Usage)mUsage
		);
		src = images[0]->getPixelBox(i, mip);
		buffer->blitFromMemory(src, dst);
		  (UpdateSubresource method)
		  */
/*
		D3D11_SUBRESOURCE_DATA init_data;
		ZeroMemory(&init_data, sizeof(init_data));
		init_data.pSysMem = image_data;
		init_data.SysMemPitch = desc.Width * 4; //static_cast<UINT>(rowPitch);
		init_data.SysMemSlicePitch = 0; // static_cast<UINT>(imageSize);
		hr = m_renderer->get_device()->CreateTexture2D(&desc, &init_data, &m_texture);

		if (!m_texture)
			return;

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
		ZeroMemory(&srv_desc, sizeof(srv_desc));
		srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srv_desc.Texture2D.MipLevels = desc.MipLevels;
		srv_desc.Texture2D.MostDetailedMip = 0;
		hr = m_renderer->get_device()->CreateShaderResourceView(m_texture, &srv_desc, &m_shader_reesource_texture_view);
		*/
	}
}