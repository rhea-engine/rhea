#include <d3d11.h>

#include "D3D11Texture.h"
#include <Psio/Log.h>

namespace hippios
{
	D3D11Texture::D3D11Texture(TextureDesc& tex_desc, Renderer* renderer) :
		Texture(tex_desc)
	{
		m_renderer = (D3D11Renderer*)renderer;

		// Create texture
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = m_desc.width;
		desc.Height = m_desc.height;
		desc.MipLevels = 1;
		desc.ArraySize = m_desc.array_size;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		if (m_desc.type == TextureDesc::TextureType::TEXTURE_CUBE || m_desc.type == TextureDesc::TextureType::TEXTURE_CUBE_ARRAY) {
			desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;			
		}

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
		if (m_desc.type == TextureDesc::TextureType::TEXTURE_2D || m_desc.type == TextureDesc::TextureType::TEXTURE_2D_ARRAY)
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		else if (m_desc.type == TextureDesc::TextureType::TEXTURE_CUBE || m_desc.type == TextureDesc::TextureType::TEXTURE_CUBE_ARRAY)
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		else {
			printf("Texture type not supported\n");
			return;
		}
		srv_desc.Texture2D.MostDetailedMip = 0;
		srv_desc.Texture2D.MipLevels = desc.MipLevels;
		hr = m_renderer->get_device()->CreateShaderResourceView(m_texture, &srv_desc, &m_shader_resource_texture_view);
	}

	D3D11Texture::~D3D11Texture()
	{
		if (m_texture)
			m_texture->Release();
		if (m_shader_resource_texture_view)
			m_shader_resource_texture_view->Release();
	}

	void D3D11Texture::loadImpl(int index, unsigned int width, unsigned int height, unsigned char* image_data)
	{
		if (!width || !height || !image_data)
		{
			return;
		}

		if (!m_texture) {
			return;			
		}

		unsigned int rowPitch = width * 4;

		D3D11_BOX destBox;
		destBox.left = (UINT)0;
		destBox.right = (UINT)(0 + width);
		destBox.top = (UINT)0;
		destBox.bottom = (UINT)(0 + height);
		destBox.front = 0;
		destBox.back = 1; 
		m_renderer->get_device_context()->UpdateSubresource(m_texture, index, &destBox, image_data, rowPitch, 0);
	}

	void D3D11Texture::loadCubemapFace(int face_index, Texture* src_texture)
	{
		D3D11Texture* source_texture = static_cast<D3D11Texture*>(src_texture);

		if (!src_texture || !m_texture || !source_texture->m_texture) {
			return;
		}
		
		// Ensure that both textures are created with the same dimensions and formats.
		D3D11_TEXTURE2D_DESC destDesc, srcDesc;
		m_texture->GetDesc(&destDesc);
		source_texture->m_texture->GetDesc(&srcDesc);

		if (destDesc.Width != srcDesc.Width || destDesc.Height != srcDesc.Height || destDesc.Format != srcDesc.Format) {
			LOG(LOG_ERROR) << "D3D11Texture::loadCubemapFace: Source and destination textures must have the same dimensions and format.";
			return;
		}

		m_renderer->get_device_context()->CopySubresourceRegion(
			m_texture,          // Destination texture (cubemap)
			D3D11CalcSubresource(0, face_index, 1), // Destination subresource index
			0, 0, 0,            // Destination coordinates (left, top, front)
			source_texture->m_texture,      // Source texture (face)
			0,                  // Source subresource index
			nullptr             // Source box (nullptr means the entire texture)
		);
	}
}