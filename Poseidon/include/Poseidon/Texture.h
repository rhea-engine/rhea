/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef POSEIDON_TEXTURE_H
#define POSEIDON_TEXTURE_H

#include "Hippios/Renderer.h"
#include "Hippios/Texture.h"

#include <Psio/Image/Image.h>

namespace poseidon
{
	class Texture : public Image
	{
	protected:
		hippios::Renderer* m_renderer = 0;
		std::unique_ptr<hippios::Texture> m_hw_texture;
		virtual void createTexture();
	public:
		void bind(hippios::Renderer *engine);
		
		// load an image from filesystem
		bool load(const std::string& file_name) override;

		// load an image from buffer
		virtual bool load(int index, unsigned char* buf, size_t buflen);

		// load rgba image from buffer
		virtual bool load(int index, unsigned int width, unsigned int height, unsigned char* image_data);

		// load an image from existing texture (not implemented for base class)
		virtual bool load(int index, Texture* tex);

		hippios::Texture* get_texture() { return m_hw_texture.get(); };
		virtual void setNumImages(int num_images) {}
	};
}
#endif // !POSEIDON_TEXTURE_H
