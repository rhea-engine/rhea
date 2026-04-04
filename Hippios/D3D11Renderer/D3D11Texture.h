#ifndef HIPPIOS_D3D11TEXTURE_H
#define HIPPIOS_D3D11TEXTURE_H

#include <d3d11.h>

#include "Hippios/Renderer.h"
#include "Hippios/Texture.h"
#include "D3D11Renderer.h"

namespace hippios
{
	class D3D11Texture : public Texture
	{
	protected:
		D3D11Renderer* m_renderer;
		ID3D11Texture2D* m_texture = nullptr;
		ID3D11ShaderResourceView* m_shader_resource_texture_view = nullptr;
	public:
		D3D11Texture(TextureDesc& desc, Renderer* renderer);
		~D3D11Texture();
		void loadImpl(int index, unsigned int width, unsigned int height, unsigned char* image_data) override;
		void loadCubemapFace(int faceIndex, Texture* src_texture);
		ID3D11ShaderResourceView* get() { return m_shader_resource_texture_view; };
	};
}

#endif // ! HIPPIOS_D3D11TEXTURE_H