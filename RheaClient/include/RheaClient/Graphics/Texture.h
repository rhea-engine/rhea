/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef REA_GRAPHICS_TEXTURE_H
#define REA_GRAPHICS_TEXTURE_H

#include <cstdint>

#include <Psio/Image/Image.h>
#include <Poseidon/Core.h>
#include <Rhea/Resources/ImageResource.h>

namespace rhea
{
	class Unit;
}

namespace rhea::client {	

	class GraphicsTexture
	{
	public:
		GraphicsTexture(poseidon::Core* core, rhea::ImageResource::TextureType texture_type, int num_textures);

		// create a texture from buffer
		poseidon::Texture* createTexture(int index, unsigned char* buf, size_t buflen);

		// create a texture from a texture array
		poseidon::Texture* createTexture(int index, GraphicsTexture* texture);

		// get the Poseidon Texture
		poseidon::Texture* get() { return m_img; }

	protected:
		poseidon::Core* m_core;
		poseidon::Texture* m_img;
		rhea::ImageResource::TextureType m_type;
	};
}

#endif // !REA_GRAPHICS_TEXTURE_H
