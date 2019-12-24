#ifndef HIPPIOS_RENDERER_H
#define HIPPIOS_RENDERER_H

#include <string>

#include "RenderListener.h"

namespace Hippios
{
	class HippiosWindow;
	class VertexBuffer;
	class IndexBuffer;
	class GraphicsBuffer;
	class Texture;

	class Renderer
	{
	public:
		virtual HippiosWindow* createWindow() = 0;
		virtual int render() = 0;
		virtual bool drawBuffer(VertexBuffer* v_buffer, IndexBuffer* i_buffer, Texture* texture) = 0;
		virtual VertexBuffer* createVertexBuffer(size_t vertexSize, size_t numVertices) = 0;
		virtual IndexBuffer* createIndexBuffer(size_t vertex_size, size_t num_faces) = 0;
		virtual GraphicsBuffer* createGraphicsBuffer(size_t size_in_bytes) = 0;
		virtual Texture* createTexture() = 0;
		virtual void clearPass() = 0;
		virtual int presentPass() = 0;
		virtual bool addMaterial(std::string filename, char* shader_type, char* entry_point, int vertex_type) = 0;
	};
}
#endif // !HIPPIOS_RENDERER_H
