#ifndef HIPPIOS_RENDERER_H
#define HIPPIOS_RENDERER_H

#include <string>

#include "RenderListener.h"

namespace Psio {
	class Engine;
}

namespace Hippios
{
	class HippiosWindow;
	class VertexBuffer;
	class IndexBuffer;
	class GraphicsBuffer;
	class Texture;
	class GPUProgram;

	class Renderer
	{
	public:
		virtual HippiosWindow* createWindow(Psio::Engine *psio) = 0;
		virtual int render() = 0;
		virtual bool drawBuffer(VertexBuffer* v_buffer, IndexBuffer* i_buffer, Texture* texture) = 0;
		virtual VertexBuffer* createVertexBuffer(size_t vertexSize, size_t numVertices) = 0;
		virtual IndexBuffer* createIndexBuffer(size_t vertex_size, size_t num_faces) = 0;
		virtual GraphicsBuffer* createGraphicsBuffer(size_t size_in_bytes) = 0;
		virtual Texture* createTexture() = 0;
		virtual void clearPass() = 0;
		virtual int presentPass() = 0;
		virtual GPUProgram* addMaterial(std::string filename, char* shader_type, char* entry_point, int vertex_type) = 0;
		virtual GPUProgram* addMaterial(unsigned char* buf, int buflen, char* shader_type, char* entry_point, int vertex_type) = 0;
		
		// bind the current vertex and index buffer
		virtual void bindBuffers(VertexBuffer* vb, IndexBuffer* ib) = 0;

		// set the current Material
		virtual void setVertexShader(GPUProgram* material, int topology) = 0;
		virtual void setPixelShader(GPUProgram* material) = 0;

		// set current Texture
		virtual void setTexture(Texture* texture, int slot = 0) = 0;

		// draw without index
		virtual void draw(int size, int start_vtx) = 0;

		// draw index with current buffers
		virtual void drawIndexed(int size, int start_idx, int start_vtx) = 0;

		// set gpu parameters

		// bind a buffer to a slot and shader phase
		virtual void bindGPUBuffer(GraphicsBuffer* buffer, int slot = 0, char* stage = "VS") = 0;

		// set up a buffer for the gpu input
		virtual GraphicsBuffer* initGPUBuffer(size_t size) = 0;
	};
}
#endif // !HIPPIOS_RENDERER_H
