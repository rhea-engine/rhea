#ifndef HIPPIOS_RENDERER_H
#define HIPPIOS_RENDERER_H

#include <string>

#include "BaseTypes.h"
#include "RenderListener.h"
#include "TextureDesc.h"
#include "PipelineState.h"
#include "BlendState.h"
#include "Sampler.h"

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

		virtual ~Renderer() {}
		virtual HippiosWindow* createWindow(Psio::Engine *psio) = 0;
		virtual int render() = 0;
		virtual VertexBuffer* createVertexBuffer(size_t vertexSize, size_t numVertices, bool dynamic) = 0;
		virtual IndexBuffer* createIndexBuffer(size_t vertex_size, size_t num_faces, bool dynamic) = 0;
		virtual GraphicsBuffer* createGraphicsBuffer(size_t size_in_bytes) = 0;
		virtual Texture* createTexture(TextureDesc& desc) = 0;
		virtual void clearPass() = 0;
		virtual int presentPass() = 0;
		virtual GPUProgram* addMaterial(std::string filename, char* shader_type, char* entry_point) = 0;
		virtual GPUProgram* addMaterial(unsigned char* buf, int buflen, char* shader_type, char* entry_point) = 0;

		// add a pipleline state object
		virtual PipelineStateObject* createPipelineState(PipelineStateObjectDesc* desc) = 0;

		// set the active pipeline state
		virtual void setPipelineState(PipelineStateObject* pso) = 0;
		
		// bind the current vertex and index buffer
		virtual void bindBuffers(VertexBuffer* vb, IndexBuffer* ib) = 0;

		// set the current Material
		virtual void setVertexShader(GPUProgram* material) = 0;
		virtual void setPixelShader(GPUProgram* material) = 0;

		// set current Texture
		virtual void setTexture(Texture* texture, int slot = 0) = 0;

		// draw without index
		virtual void draw(int size, int start_vtx) = 0;

		// draw index with current buffers
		virtual void drawIndexed(int size, int start_idx, int start_vtx) = 0;

		// set the blend state with the given blend factor and mask
		virtual void setBlendState(BlendState* state, float blend_factor[4], unsigned int mask) = 0;

		// set the stencil state to state with ref value
		virtual void setStencilState(StencilState* state, int stencil_ref) = 0;

		// get the stencil state with the given name
		virtual StencilState* getStencilState(std::string name) = 0;

		// create a new stencil state
		virtual StencilState* createStencilState(std::string name, DepthStencilStateDesc& desc) = 0;

		// create a new sampler
		virtual GPUHandle* createSampler(std::string name, SamplerDesc* desc) = 0;
		// get a sampler
		virtual GPUHandle* getSampler(std::string name) = 0;
		// set sampler as active
		virtual void setSampler(GPUHandle* handle) = 0;

		// set gpu parameters

		// bind a buffer to a slot and shader phase
		virtual void bindGPUBuffer(GraphicsBuffer* buffer, int slot = 0, char* stage = "VS") = 0;

		// set up a buffer for the gpu input
		virtual GraphicsBuffer* initGPUBuffer(size_t size) = 0;
	};
}
#endif // !HIPPIOS_RENDERER_H
