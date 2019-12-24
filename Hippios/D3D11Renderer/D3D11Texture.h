#ifndef HIPPIOS_D3D11TEXTURE_H
#define HIPPIOS_D3D11TEXTURE_H

#include <d3d11.h>

#include "Hippios/Renderer.h"
#include "Hippios/Texture.h"
#include "D3D11Renderer.h"

namespace Hippios
{
	class D3D11Texture : public Texture
	{
	protected:
		D3D11Renderer* m_renderer;
		ID3D11Texture2D* m_texture = nullptr;
		ID3D11ShaderResourceView* m_shader_resource_texture_view = nullptr;
	public:
		D3D11Texture(Renderer* renderer);
		void loadImpl(unsigned int width, unsigned int height, unsigned char* image_data);
		ID3D11ShaderResourceView* get() { return m_shader_resource_texture_view; };
	};
}

#endif // ! HIPPIOS_D3D11TEXTURE_H