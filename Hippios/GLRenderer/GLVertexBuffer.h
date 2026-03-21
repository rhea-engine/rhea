#ifndef GL_VERTEXBUFFER_H
#define GL_VERTEXBUFFER_H

#include <Hippios/VertexBuffer.h>
#include "GLGraphicsBuffer.h"
#include <memory>

namespace Hippios
{
	class GLVertexBuffer : public VertexBuffer
	{
	public:
		bool is_dirty;

		GLVertexBuffer(size_t vertex_size, size_t num_vertices, bool dynamic);

		void* Lock(size_t offset, size_t length) override;
		void Unlock() override;
		void ReadData(size_t offset, size_t length, void* dest) override;
		void WriteData(size_t offset, size_t length, const void* source, bool discardBuffer = false, bool mapped = true) override;
		void update(const void* source) override;

		GLuint get_buffer() const { return m_buffer->get_buffer(); }

	private:
		std::unique_ptr<GLGraphicsBuffer> m_buffer;
	};
}
#endif // !GL_VERTEXBUFFER_H