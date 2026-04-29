/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef GL_GRAPHICSBUFFER_H
#define GL_GRAPHICSBUFFER_H

#include <Hippios/GraphicsBuffer.h>
#include "GLIncludes.h"

namespace hippios
{
	class GLGraphicsBuffer : public GraphicsBuffer
	{
	public:
		GLGraphicsBuffer(GLenum target, size_t size_in_bytes, bool dynamic = true);
		~GLGraphicsBuffer();

		void* Lock(size_t offset, size_t length) override;
		void Unlock() override;
		void ReadData(size_t offset, size_t length, void* dest) override;
		void WriteData(size_t offset, size_t length, const void* source, bool discardBuffer = false, bool mapped = true) override;
		void update(const void* source) override;

		GLuint get_buffer() const { return m_buffer; }
		GLenum get_target() const { return m_target; }

	protected:
		GLuint m_buffer = 0;
		GLenum m_target;
		bool m_dynamic;
	};
}
#endif // !GL_GRAPHICSBUFFER_H