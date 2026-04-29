/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Poseidon/Cubemap.h"

namespace poseidon
{
    bool Cubemap::load(int index, unsigned char* buf, size_t buflen)
    {
        if (!m_hw_texture) {
            hippios::TextureDesc desc;
            desc.width = m_width;
            desc.height = m_height;
            desc.type = hippios::TextureDesc::TextureType::TEXTURE_CUBE;
            desc.depth = 1;
            desc.array_size = m_num_images;
            m_hw_texture = m_renderer->createTexture(desc);
        }

        Image* rgba = ConvertToRGBA();
        if (rgba) {
            m_hw_texture->loadImpl(index, m_width, m_height, rgba->get_data());
            return true;
        }
        return false;
    }

    bool Cubemap::load(int index, Texture* tex)
    {
        if (!m_hw_texture) {
			m_width = tex->getWidth();
			m_height = tex->getHeight();
            createTexture();
        }
        m_hw_texture->loadCubemapFace(index, tex->get_texture());
        return true;
    }
 
    void Cubemap::createTexture()
    {
        hippios::TextureDesc desc;
        desc.width = m_width;
        desc.height = m_height;
        desc.type = hippios::TextureDesc::TextureType::TEXTURE_CUBE;
        desc.depth = 1;
        desc.array_size = m_num_images;
        m_hw_texture = m_renderer->createTexture(desc);
    }
}