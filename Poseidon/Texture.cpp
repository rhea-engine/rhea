#include "Texture.h"

namespace Poseidon
{
	void Texture::bind(Hippios::Engine* engine)
	{
		m_renderer = engine->getRenderer();
	}

	bool Texture::load(const std::string& file_name)
	{
		if (Image::load(file_name)) {
			m_hw_texture = m_renderer->createTexture();

			Image* rgba = ConvertToRGBA();

			m_hw_texture->loadImpl(m_width, m_height, rgba->get_data());
			return true;
		}
		else
			return false;
	}
}