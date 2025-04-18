#include "Texture.h"

namespace Poseidon
{
	void Texture::bind(Hippios::Renderer* renderer)
	{
		m_renderer = renderer;
	}

	bool Texture::load(const std::string& file_name)
	{
		if (Image::load(file_name)) {
			createTexture();

			Image* rgba = ConvertToRGBA();

			m_hw_texture->loadImpl(0, m_width, m_height, rgba->get_data());
			return true;
		}
		else
			return false;
	}
	
	bool Texture::load(int index, unsigned char* buf, size_t buflen)
	{
		if (Image::load(buf, buflen)) {
			createTexture();

			Image* rgba = ConvertToRGBA();

			m_hw_texture->loadImpl(index, m_width, m_height, rgba->get_data());
			return true;
		}
		else
			return false;
	}

	bool Texture::load(int index, Texture* tex)
	{
		abort();
	}

	void Texture::createTexture()
	{
		Hippios::TextureDesc desc;
		desc.width = m_width;
		desc.height = m_height;
		desc.type = Hippios::TextureDesc::TextureType::TEXTURE_2D;
		desc.depth = 1;
		desc.array_size = 1;
		m_hw_texture = m_renderer->createTexture(desc);
	}
}