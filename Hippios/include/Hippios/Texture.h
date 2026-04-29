/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HIPPIOS_TEXTURE_H
#define HIPPIOS_TEXTURE_H

#include "TextureDesc.h"

namespace hippios
{
	class Renderer;

	class Texture
	{
	public:
		Texture(TextureDesc& desc) :
			m_desc(desc)
		{}
		virtual ~Texture() = default;

		virtual void loadImpl(int index, unsigned int width, unsigned int height, unsigned char* image_data) = 0;
		virtual void loadCubemapFace(int faceIndex, Texture* src_texture) = 0;

		TextureDesc& getDesc() { return m_desc; }

	protected:
		TextureDesc m_desc;
	};
}

#endif // ! HIPPIOS_TEXTURE_H