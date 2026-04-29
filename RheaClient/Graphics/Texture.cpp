/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <RheaClient/Graphics/Texture.h>
#include <Rhea/Unit.h>
#include <Rhea/Resources/Resource.h>
#include <Psio/Log.h>
#include <Rhea/Resources/ImageResource.h>
#include <Poseidon/Texture.h>

namespace rhea::client {
	GraphicsTexture::GraphicsTexture(poseidon::Core* core, rhea::ImageResource::TextureType texture_type, int num_textures) :
		m_type(texture_type),
		m_core(core),
		m_img(nullptr)
	{
		if (num_textures > 0) {
			if (texture_type == rhea::ImageResource::TextureType::TEXTURE_2D)
				m_img = m_core->createTexture(0, num_textures);
			else
				m_img = m_core->createTexture(1, num_textures);
		}
	}

	poseidon::Texture* GraphicsTexture::createTexture(int index, unsigned char* buf, size_t buflen)
	{		
		m_img->load(index, (unsigned char*)buf, buflen);
		return m_img;
	}

	poseidon::Texture* GraphicsTexture::createTexture(int index, GraphicsTexture* texture)
	{
		m_img->load(index, texture->m_img);
		return m_img;
	}	
}