/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef GL_DEPTHBUFFER_H
#define GL_DEPTHBUFFER_H

#include <Hippios/DepthBuffer.h>
#include "GLIncludes.h"

namespace hippios
{
	class GLDepthBuffer : public DepthBuffer
	{
	public:
		GLDepthBuffer(unsigned int bit_depth, unsigned int width, unsigned int height);
		~GLDepthBuffer();

		GLuint getRenderbuffer() const { return m_renderbuffer; }

	private:
		GLuint m_renderbuffer = 0;
	};
}
#endif // !GL_DEPTHBUFFER_H