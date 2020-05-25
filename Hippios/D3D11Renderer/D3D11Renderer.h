#ifndef HIPPIOS_D3D11RENDERER_H
#define HIPPIOS_D3D11RENDERER_H

#include <d3d11.h>
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <string>

#include "Hippios/Engine.h"
#include "Hippios/Renderer.h"
#include "ComPtr.h"
#include "D3D11DepthBuffer.h"

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

	class D3D11Renderer : public Renderer
	{
	private:
		D3D11Window* m_win = 0;

		D3D_DRIVER_TYPE mDriverType; // should be XXX_HARDWARE, XXX_SOFTWARE or XXX_WARP, never XXX_UNKNOWN or XXX_NULL
		D3D_FEATURE_LEVEL mFeatureLevel;
		D3D_FEATURE_LEVEL mMinRequestedFeatureLevel;
		D3D_FEATURE_LEVEL mMaxRequestedFeatureLevel;

		/// Direct3D rendering device
		ID3D11Device* m_device = 0;		

		// shaders
		D3D11HLSL* mVShader = 0;
		D3D11HLSL* mPShader = 0;

		HRESULT createDevice(D3D_DRIVER_TYPE driverType, D3D_FEATURE_LEVEL minFL, D3D_FEATURE_LEVEL maxFL, D3D_FEATURE_LEVEL* pFeatureLevel);		
		ID3D11DeviceContext* m_device_context = 0;
		IDXGISwapChain* m_pDXGISwapChain = 0;
		ID3D11RenderTargetView* mBackbuffer = 0;    // the pointer to our back buffer
		D3D11DepthBuffer* m_depth_buffer = 0;
		ID3D11SamplerState* m_sampler_state = 0;
		ID3D11Buffer* m_cb_per_object_buffer = 0;

		void createSwapChain();
		void setRenderTarget();

		//TODO allow more than one mesh
		D3D11VertexBuffer* m_vb = 0;
		D3D11IndexBuffer* m_ib = 0;
	public:
		D3D11Renderer();
		D3D11Renderer(ID3D11Device *device, ID3D11DeviceContext *device_context);
		~D3D11Renderer();

		// steps for initializing Renderer
		void init();

		// getters
		ID3D11DeviceContext* get_device_context() { return m_device_context; }
		ID3D11Device* get_device() { return m_device; }
		IDXGISwapChain* get_swapchain() { return m_pDXGISwapChain; };
		ID3D11RenderTargetView* get_backbuffer() {
			return mBackbuffer;
		}
		D3D11DepthBuffer* get_depth_buffer() {
			return m_depth_buffer;
		}
		ID3D11SamplerState* get_default_samplerstate() {
			return m_sampler_state;
		}

		// create and use objects

		HippiosWindow* createWindow(Psio::Engine *psio);
		void set_window(HippiosWindow* win);
		VertexBuffer* createVertexBuffer(size_t vertexSize, size_t numVertices);
		IndexBuffer* createIndexBuffer(size_t vertex_size, size_t num_faces);
		GraphicsBuffer* createGraphicsBuffer(size_t size_in_bytes);
		bool drawBuffer(VertexBuffer* v_buffer, IndexBuffer* i_buffer = 0, Texture* texture = 0);
		Texture* createTexture();
		GPUProgram* addMaterial(std::string filename, char* shader_type, char* entry_point, int vertex_type);

		// set current material
		void setVertexShader(GPUProgram* material, int topology);
		void setPixelShader(GPUProgram* material);

		// current texture 
		void setTexture(Texture* texture);

		void draw(int size, int start_vtx);
		void drawIndexed(int size, int start_idx, int start_vtx);

		void setGPUParameter(void* param);

		// graphics param buffer
		GraphicsBuffer* initGPUBuffer(size_t size);
		void setGPUBuffer(GraphicsBuffer* buffer);
		ID3D11Buffer* getObjBuffer() {
			return m_cb_per_object_buffer;
		}

		// render passes
		void update();
		void bindBuffers(VertexBuffer* vb, IndexBuffer* ib);
		void clearPass();
		int presentPass();
		int render();

	protected:
		Texture* m_current_texture;
	};
}
#endif // !HIPPIOS_D3D11RENDERER_H
