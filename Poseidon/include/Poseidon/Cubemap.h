/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef POSEIDON_CUBEMAP_H
#define POSEIDON_CUBEMAP_H

#include "Texture.h"

namespace poseidon
{
	class Cubemap : public Texture
	{
	public:
		bool load(int index, unsigned char* buf, size_t buflen) override;
		bool load(int index, Texture* tex) override;
		void setNumImages(int num_images) { m_num_images = num_images; }

	protected:
		void createTexture() override;
		int m_num_images = 1;
	};
}
#endif // !POSEIDON_CUBEMAP_H
