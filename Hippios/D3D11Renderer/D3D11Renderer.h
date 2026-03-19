#ifndef HIPPIOS_D3D11RENDERER_H
#define HIPPIOS_D3D11RENDERER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <string>
#include <unordered_map>
#include <memory>

#include "Hippios/Renderer.h"
#include "ComPtr.h"
#include "D3D11DepthBuffer.h"
#include "D3D11BaseTypes.h"
#include "D3D11ShaderProgram.h"

class D3D11Driver;
namespace Hippios
{
	class D3D11Window;
	class HippiosWindow;
	class D3D11HLSL;
	class GraphicsBuffer;
	class VertexBuffer;
	class D3D11VertexBuffer;
	class D3D11IndexBuffer;
	class Texture;	
	class D3D11StencilState;	

	class D3D11Renderer : public Renderer
	{
	private:
		D3D_DRIVER_TYPE mDriverType;
		D3D_FEATURE_LEVEL mFeatureLevel;
		D3D_FEATURE_LEVEL mMinRequestedFeatureLevel;
		D3D_FEATURE_LEVEL mMaxRequestedFeatureLevel;

		/// Direct3D rendering device
		ID3D11Device* m_device = 0;		
		ID3D11DeviceContext* m_device_context = 0;

		// shaders
		D3D11HLSL* mVShader = 0;
		D3D11HLSL* mPShader = 0;

		HRESULT createDeviceAndSwapChain(D3D_DRIVER_TYPE driverType, D3D_FEATURE_LEVEL minFL, D3D_FEATURE_LEVEL maxFL, D3D_FEATURE_LEVEL* pFeatureLevel, NativeWindow* win, IDXGISwapChain** swap_chain);

		// last buffers that where bound
		D3D11VertexBuffer* m_last_vb = 0;
		D3D11IndexBuffer* m_last_ib = 0;
	public:
		D3D11Renderer();
		D3D11Renderer(ID3D11Device *device, ID3D11DeviceContext *device_context);
		~D3D11Renderer();

		// steps for initializing Renderer		
		std::unique_ptr<RenderTarget> createRenderTarget(int width, int height, SwapChain* swap_chain) override;
		std::unique_ptr<RenderTarget> createRenderTarget(const RenderTargetDesc& desc) override;

		// getters
		ID3D11DeviceContext* get_device_context() { return m_device_context; }
		ID3D11Device* get_device() { return m_device; }

		ID3D11SamplerState* get_default_samplerstate() {
			if (m_sampler_states.size())
				return m_sampler_states[0];
			else
				return nullptr;
		}
		
		// create and use objects

		std::unique_ptr<VertexBuffer> createVertexBuffer(size_t vertexSize, size_t numVertices, bool dynamic = false) override;
		std::unique_ptr<IndexBuffer> createIndexBuffer(size_t vertex_size, size_t num_faces, bool dynamic = false) override;
		std::unique_ptr<GraphicsBuffer> createGraphicsBuffer(size_t size_in_bytes) override;
		std::unique_ptr<Texture> createTexture(TextureDesc& desc) override;
		ShaderProgram* createShader(const ShaderProgramDesc& desc) override;
		virtual ShaderProgram* createShader(const ShaderProgramSourceDesc& desc) override;

		std::unique_ptr<SwapChain> createSwapChain(NativeWindow* win) override;

		void setBlendState(BlendState* state, float* blend_factor, unsigned int mask) override;

		std::unique_ptr<PipelineStateObject> createPipelineState(const PipelineStateObjectDesc& desc) override;
		void setPipelineState(PipelineStateObject* pso) override;		

		// stencil states
		StencilState* getStencilState(const std::string& name) override;
		StencilState* createStencilState(const std::string& name);
		StencilState* createStencilState(const std::string& name, const DepthStencilStateDesc& desc) override;
		void setStencilState(StencilState* state, int stencil_ref) override;

		// create a new sampler
		bool createSampler(const std::string& name, const SamplerDesc& desc) override;
		// get a sampler
		GPUHandle* getSampler(std::string& name) override;
		virtual void setSampler(GPUHandle* handle) override;

		// rasterizer state
		void setRasterizerState(ID3D11RasterizerState* state);

		// current texture 
		void setTexture(Texture* texture, int slot = 0) override;

		void draw(int size, int start_vtx) override;
		void drawIndexed(int size, int start_idx, int start_vtx) override;

		// graphics param buffer

		// setup and create a buffer for shaders
		GraphicsBuffer* initGPUBuffer(size_t size) override;

		// bind a buffer to a slot and shader phase
		void bindGPUBuffer(GraphicsBuffer* buffer, int slot = 0, ShaderStage stage = ShaderStage::Vertex) override;
		
		// render passes		
		void bindBuffers(VertexBuffer* vb, IndexBuffer* ib) override;
		void clearPass(RenderTarget* render_target) override;
		int presentPass(SwapChain* swap_chain) override;

	protected:
		Texture* m_current_texture;		
		PipelineStateObject* m_current_pso;
		std::unordered_map<std::string, std::unique_ptr<D3D11StencilState>> m_stencil_states;
		D3D11StencilState* m_current_stencil_state;
		ID3D11RasterizerState* m_current_raster_state;
		std::vector<ID3D11InputLayout*> m_input_layouts;
		std::vector<ID3D11SamplerState*> m_sampler_states;
		std::unordered_map<std::string, D3D11GPUHandle> m_samplers;
	};
}
#endif // !HIPPIOS_D3D11RENDERER_H
