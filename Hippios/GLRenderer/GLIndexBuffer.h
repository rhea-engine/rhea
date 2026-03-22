#ifndef GL_INDEXBUFFER_H
#define GL_INDEXBUFFER_H

#include <Hippios/IndexBuffer.h>
#include "GLGraphicsBuffer.h"
#include <memory>

namespace Hippios
{
	class GLIndexBuffer : public IndexBuffer
	{
	public:
		bool is_dirty;

		GLIndexBuffer(size_t index_stride, size_t num_faces, bool dynamic);

		void* Lock(size_t offset, size_t length) override;
		void Unlock() override;
		void ReadData(size_t offset, size_t length, void* dest) override;
		void WriteData(size_t offset, size_t length, const void* source, bool discardBuffer = false, bool mapped = true) override;
		void update(const void* source) override;

		GLuint get_buffer() const { return m_buffer->get_buffer(); }
		GLenum getIndexType() const { return m_index_type; }

	private:
		std::unique_ptr<GLGraphicsBuffer> m_buffer;
		GLenum m_index_type = GL_UNSIGNED_INT;
	};
}
#endif // !GL_INDEXBUFFER_H