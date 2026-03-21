#ifndef HIPPIOS_GLRENDERER_H
#define HIPPIOS_GLRENDERER_H

#include <Hippios/Renderer.h>
#include "GLBaseTypes.h"
#include "GLStencilState.h"

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

#include "GLIncludes.h"

namespace Hippios
{
	class GLVertexBuffer;
	class GLIndexBuffer;
	class GLShader;
	class GLSamplerState;

	class GLRenderer : public Renderer
	{
	public:
		GLRenderer();
		~GLRenderer();
		
		std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetDesc& desc) override;
		std::unique_ptr<VertexBuffer> createVertexBuffer(size_t vertexSize, size_t numVertices, bool dynamic = false) override;
		std::unique_ptr<IndexBuffer> createIndexBuffer(size_t vertex_size, size_t num_faces, bool dynamic = false) override;
		std::unique_ptr<GraphicsBuffer> createGraphicsBuffer(size_t size_in_bytes) override;
		std::unique_ptr<Texture> createTexture(TextureDesc& desc) override;
		ShaderProgram* createShader(const ShaderProgramDesc& desc) override;
		ShaderProgram* createShader(const ShaderProgramSourceDesc& desc) override;
		std::unique_ptr<SwapChain> createSwapChain(NativeWindow* win) override;

		void setBlendState(BlendState* state, float* blend_factor, unsigned int mask) override;

		std::unique_ptr<PipelineStateObject> createPipelineState(const PipelineStateObjectDesc& desc) override;
		void setPipelineState(PipelineStateObject* pso) override;

		StencilState* getStencilState(const std::string& name) override;
		StencilState* createStencilState(const std::string& name) override;
		StencilState* createStencilState(const std::string& name, const DepthStencilStateDesc& desc) override;
		void setStencilState(StencilState* state, int stencil_ref) override;

		SamplerState* createSampler(const SamplerDesc& desc) override;
		void setSampler(int slot, SamplerState* sampler, ShaderStage stage) override;

		void setTexture(Texture* texture, int slot = 0) override;

		void draw(int size, int start_vtx) override;
		void drawIndexed(int size, int start_idx, int start_vtx) override;

		GraphicsBuffer* initGPUBuffer(size_t size) override;
		void bindGPUBuffer(GraphicsBuffer* buffer, int slot = 0, ShaderStage stage = ShaderStage::Vertex) override;

		void bindBuffers(VertexBuffer* vb, IndexBuffer* ib) override;
		void clearPass(RenderTarget* render_target) override;
		int presentPass(SwapChain* swap_chain) override;

	private:
		bool initOpenGLContext(HWND hwnd);

		HGLRC m_glrc = nullptr;
		HDC m_hdc = nullptr;

		GLVertexBuffer* m_last_vb = nullptr;
		GLIndexBuffer* m_last_ib = nullptr;

		Texture* m_current_texture = nullptr;
		PipelineStateObject* m_current_pso = nullptr;
		GLStencilState* m_current_stencil_state = nullptr;

		GLenum m_current_topology = GL_TRIANGLES;

		std::unordered_map<std::string, std::unique_ptr<GLStencilState>> m_stencil_states;
		std::vector<std::unique_ptr<GLSamplerState>> m_sampler_states;
	};
}
#endif // !HIPPIOS_GLRENDERER_H