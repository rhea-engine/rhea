#ifndef REA_GRAPHICS_TEXTURE_H
#define REA_GRAPHICS_TEXTURE_H

#include <cstdint>

#include <Psio/Image/Image.h>
#include <Poseidon/Core.h>
#include <Rhea/Fabrics/Media/ImageResource.h>

namespace Rhea
{
	class Unit;

	class GraphicsTexture
	{
	public:
		GraphicsTexture(Poseidon::Core* core, ImageResource::TextureType texture_type, int num_textures);

		// create a texture from buffer
		Poseidon::Texture* createTexture(int index, unsigned char* buf, size_t buflen);

		// create a texture from a texture array
		Poseidon::Texture* createTexture(int index, GraphicsTexture* texture);

		// get the Poseidon Texture
		Poseidon::Texture* get() { return m_img; }

	protected:
		Poseidon::Core* m_core;
		Poseidon::Texture* m_img;
		ImageResource::TextureType m_type;
	};
}

#endif // !REA_GRAPHICS_TEXTURE_H
