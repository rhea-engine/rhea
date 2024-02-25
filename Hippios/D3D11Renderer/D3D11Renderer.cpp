#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#if defined(DEBUG) || defined(_DEBUG)
#include <dxgi1_3.h>
#include <dxgidebug.h>
#endif
#include <iostream>
#include <assert.h>
using namespace std;

#include "D3D11Renderer.h"
#include "D3D11Window.h"
#include "D3D11HLSL.h"
#include "D3D11VertexBuffer.h"
#include "D3D11IndexBuffer.h"
#include "D3D11GraphicsBuffer.h"
#include "../Texture.h"
#include "D3D11Texture.h"
#include <Psio/Window.h>
#include "D3D11StencilState.h"
#include "D3D11PipelineState.h"

#pragma comment (lib, "d3d11.lib")

namespace Hippios {

	D3D11Renderer::D3D11Renderer() :
		D3D11Renderer(nullptr, nullptr)
	{
	}

	D3D11Renderer::D3D11Renderer(ID3D11Device* device, ID3D11DeviceContext* device_context) :
		m_device(device), m_device_context(device_context)
	{
		mMinRequestedFeatureLevel = D3D_FEATURE_LEVEL_9_1;
		mMaxRequestedFeatureLevel = D3D_FEATURE_LEVEL_11_1;
		mDriverType = D3D_DRIVER_TYPE_HARDWARE;
		mFeatureLevel = D3D_FEATURE_LEVEL_11_1;
		m_current_texture = nullptr;
		m_current_pso = nullptr;
		m_current_stencil_state = nullptr;
	}

	void D3D11Renderer::init()
	{	
		// create device and swap chain
		HRESULT hr = createDevice(mDriverType, mMinRequestedFeatureLevel, mMaxRequestedFeatureLevel, 0);
		if (FAILED(hr))
			return;
	}
		
	HRESULT D3D11Renderer::createDevice(D3D_DRIVER_TYPE driverType, D3D_FEATURE_LEVEL minFL, D3D_FEATURE_LEVEL maxFL, D3D_FEATURE_LEVEL* pFeatureLevel)
	{
		//IDXGIAdapter* pAdapter = (d3dDriver && driverType == D3D_DRIVER_TYPE_HARDWARE) ? d3dDriver->getDeviceAdapter() : NULL;
		IDXGIAdapter* pAdapter = NULL;

		// determine deviceFlags
		UINT deviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		// feature level
		D3D_FEATURE_LEVEL d3dLevels[] = {
			D3D_FEATURE_LEVEL_9_1,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_1
		};
		D3D_FEATURE_LEVEL* pFirstFL = d3dLevels, * pLastFL = pFirstFL + ARRAYSIZE(d3dLevels) - 1;
	/*	for (unsigned int i = 0; i < ARRAYSIZE(d3dLevels); i++)
		{
			if (minFL == d3dLevels[i])
				pLastFL = &d3dLevels[i];
			if (maxFL == d3dLevels[i])
				pFirstFL = &d3dLevels[i];
		}
		if (pLastFL < pFirstFL)
		{
			cout << " min level larger than max level for direct 3d 11";
			return 0;
		}
		*/

		// create device				
/*
		HRESULT hr = D3D11CreateDevice(pAdapter, 
			driverType, 
			NULL, 
			deviceFlags, 
			pFirstFL, 
			ARRAYSIZE(d3dLevels),
			D3D11_SDK_VERSION, 
			&m_device, 
			pFeatureLevel, 
			0);
*/
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
				
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // use 32-bit color
		desc.BufferDesc.Width = m_win->getWindow()->GetWidth();
		desc.BufferDesc.Height = m_win->getWindow()->GetHeight();
		desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // how swap chain is to be used
		// from example
		desc.BufferCount = 1;                                   // one back buffer		
		//desc.BufferCount = 2;
		desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
		desc.SampleDesc.Count = 1;      //multisampling setting
		desc.SampleDesc.Quality = 0;    //vendor-specific flag
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		//desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // TODO needs win10!
		desc.OutputWindow = (HWND)m_win->getWindow()->getHandle();	
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; 
#if defined(_DEBUG)		
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG; // If the project is in a debug build, enable the debug layer.
#endif

		HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			creationFlags,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&desc,
			&m_pDXGISwapChain,
			&m_device,
			NULL,
			&m_device_context);

		if (FAILED(hr))
		{
			// Handle device interface creation failure if it occurs.
			// For example, reduce the feature level requirement, or fail over 
			// to WARP rendering.
			cout << "could not create d3d11 device";
			return hr;
		}

		// Store pointers to the Direct3D 11.1 API device
		return hr;
	}

	void D3D11Renderer::createSwapChain()
	{
		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
		desc.Windowed = TRUE; // Sets the initial state of full-screen mode.
		desc.BufferCount = 2;
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SampleDesc.Count = 1;      //multisampling setting
		desc.SampleDesc.Quality = 0;    //vendor-specific flag
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // TODO needs win10!
		desc.OutputWindow = (HWND)m_win->getWindow()->getHandle();

		// get DXGI factory from device
		IDXGIDevice* pDXGIDevice;
		IDXGIAdapter* pDXGIAdapter;
		IDXGIFactory* mDXGIFactory;

		//HRESULT hr = m_device->QueryInterface(__uuidof(IDXGIDevice2), (void**)&pDXGIDevice);
		pDXGIDevice = (IDXGIDevice*)m_device;
		HRESULT hr = pDXGIDevice->GetAdapter(&pDXGIAdapter);

		//pDXGIAdapter->GetParent(__uuidof(pDXGIAdapter), (void**)&pDXGIAdapter);
		pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&mDXGIFactory);

		hr = mDXGIFactory->CreateSwapChain(
			(IUnknown*)m_device,
			&desc,
			&m_pDXGISwapChain
		);		
	}

	void D3D11Renderer::setRenderTarget()
	{
		// get the address of the back buffer
		ID3D11Texture2D* pBackBuffer;
		m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		if (!pBackBuffer)
			return;

		// use the back buffer address to create the render target
		m_device->CreateRenderTargetView(pBackBuffer, NULL, &mBackbuffer);
		pBackBuffer->Release();

		// create the depth buffer
		m_depth_buffer = new D3D11DepthBuffer(m_device, 24, m_win->getWindow()->GetWidth(), m_win->getWindow()->GetHeight());

		// create the default stencil states
		DepthStencilStateDesc desc;
		D3D11StencilState* stencil_std = new D3D11StencilState(m_device, m_device_context, nullptr);
		D3D11StencilState* stencil_disabled = new D3D11StencilState(m_device, m_device_context, &desc);
		m_stencil_states["default"].reset(stencil_std);
		m_stencil_states["disabled"].reset(stencil_disabled);

		// set the render target as the back buffer
		m_device_context->OMSetRenderTargets(1, &mBackbuffer, m_depth_buffer->get_view());

		// Set the viewport
		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = (FLOAT)m_win->getWindow()->GetWidth();
		viewport.Height = (FLOAT)m_win->getWindow()->GetHeight();
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		m_device_context->RSSetViewports(1, &viewport);		

		// Describe the Sample State
		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		//Create the Sample State
		HRESULT HR = m_device->CreateSamplerState(&sampDesc, &m_sampler_state);
		// set as default sampler
		m_device_context->PSSetSamplers(0, 1, &m_sampler_state);
	}

	HippiosWindow* D3D11Renderer::createWindow(Psio::Engine *psio)
	{
		// TODO: this "attaches" the window as render target - could be a list
		m_win = new D3D11Window();
		m_win->create(psio);
		init();

		// set render target
		setRenderTarget();

		return m_win;
	}

	void D3D11Renderer::set_window(HippiosWindow* win)
	{
		m_win = (D3D11Window*)win;
	}

	/// called before drawing anything
	void D3D11Renderer::update()
	{
		if (m_vb && m_vb->is_dirty) {
			UINT stride = (UINT)m_vb->get_vertex_size();
			UINT offset = 0; 
			ID3D11Buffer* buffer = m_vb->get_buffer();
			m_device_context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
			m_vb->is_dirty = false;
		}

		// set index buffer
		if (m_ib && m_ib->is_dirty) {
			ID3D11Buffer* buffer = m_ib->get_buffer();
			m_device_context->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
			m_ib->is_dirty = false;
		}
	}

	void D3D11Renderer::bindBuffers(VertexBuffer* vb, IndexBuffer* ib)
	{
		if (m_vb != vb) {
			m_vb = static_cast<D3D11VertexBuffer*>(vb);
			m_vb->is_dirty = true;
		}

		if (m_vb && m_vb->is_dirty) {
			UINT stride = (UINT)m_vb->get_vertex_size();
			UINT offset = 0;
			ID3D11Buffer* buffer = m_vb->get_buffer();
			m_device_context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
			m_vb->is_dirty = false;
		}

		if (m_ib != ib && ib) {
			m_ib = static_cast<D3D11IndexBuffer*>(ib);
			m_ib->is_dirty = true;
		}

		// set index buffer
		if (m_ib && m_ib->is_dirty) {
			ID3D11Buffer* buffer = m_ib->get_buffer();
			m_device_context->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
			m_ib->is_dirty = false;
		}
	}

	void D3D11Renderer::clearPass()
	{
		// clear the back buffer to a deep blue
		float color[4] = { 0.0f, 0.0f, 0.5f, 0.0f };
		m_device_context->ClearRenderTargetView(mBackbuffer, color);

		//Refresh the Depth/Stencil view
		m_device_context->ClearDepthStencilView(m_depth_buffer->get_view(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	int D3D11Renderer::presentPass()
	{
		// switch the back buffer and the front buffer
		HRESULT hr = m_pDXGISwapChain->Present(0, 0);
		if (FAILED(hr)) {
			printf("Swapchain.Present failed\n");
			return -1;
		}
		return 0;
	}

	int D3D11Renderer::render()
	{
		// clear the back buffer to a deep blue
		float color[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
		m_device_context->ClearRenderTargetView(mBackbuffer, color);

		// render all render targets - currently only one
		m_win->update();

		// switch the back buffer and the front buffer
		HRESULT hr = m_pDXGISwapChain->Present(0, 0);
		if (FAILED(hr)) {
			printf("Swapchain.Present failed\n");
			return -1;
		}
		return 0;
	}
	
	void D3D11Renderer::setStencilState(StencilState* state, int stencil_ref)
	{
		if (state != m_current_stencil_state) {
			m_current_stencil_state = static_cast<D3D11StencilState*>(state);
			m_current_stencil_state->setState(stencil_ref);
		}
	}

	void D3D11Renderer::setRasterizerState(ID3D11RasterizerState* state)
	{
		if (state != m_current_raster_state) {
			m_current_raster_state = state;
			m_device_context->RSSetState(m_current_raster_state);
		}
	}

	VertexBuffer* D3D11Renderer::createVertexBuffer(size_t vertexSize, size_t numVertices, bool dynamic)
	{
		D3D11VertexBuffer* buf;

		//assert(!m_vb && "TODO: only one vertex buffer allowed");
		buf = new D3D11VertexBuffer(m_device, m_device_context, vertexSize, numVertices, dynamic);
		m_vb = buf;
		return (VertexBuffer*)buf;
	}
	
	IndexBuffer* D3D11Renderer::createIndexBuffer(size_t vertex_size, size_t num_faces, bool dynamic)
	{
		D3D11IndexBuffer* buf;

		//assert(!m_ib && "TODO: only one index buffer allowed");
		buf = new D3D11IndexBuffer(m_device, m_device_context, vertex_size, num_faces, dynamic);
		m_ib = buf;
		return (IndexBuffer*)buf;
	}

	GraphicsBuffer* D3D11Renderer::createGraphicsBuffer(size_t size_in_bytes)
	{
		D3D11GraphicsBuffer* buf;

		buf = new D3D11GraphicsBuffer(m_device, m_device_context, D3D11_BIND_CONSTANT_BUFFER, size_in_bytes, false, 0);
		return (GraphicsBuffer*)buf;
	}

	D3D11Renderer::~D3D11Renderer()
	{
		m_pDXGISwapChain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode to allow D3D11 to close

		// close and release all existing COM objects

		// release input layouts
		m_device_context->IASetInputLayout(NULL);
		for (auto it = m_input_layouts.begin(); it != m_input_layouts.end(); ++it)
			(*it)->Release();

		// release the swap chain
		m_pDXGISwapChain->Release();

		// release the back buffer
		mBackbuffer->Release();

		m_device->Release();
		m_device_context->Release();
	}

	Texture* D3D11Renderer::createTexture(TextureDesc& desc)
	{
		D3D11Texture* texture = new D3D11Texture(desc, this);
		return texture;
	}

	GPUProgram* D3D11Renderer::addMaterial(std::string filename, char* shader_type, char* entry_point)
	{
		if (!strcmp(shader_type, "vs")) {
			mVShader = new D3D11HLSL(m_device, m_device_context);
			HRESULT hr = mVShader->LoadFromFile(filename, entry_point, "vs", "vs_5_0");
			//HRESULT hr = mVShader->defaultShader("VShader", "vs", "vs_5_0");
			if (FAILED(hr))
			{
				printf("Failed compiling vertex shader %08X\n", hr);
				return nullptr;
			}
			return mVShader;
		}
		else {
			mPShader = new D3D11HLSL(m_device, m_device_context);
			HRESULT hr = mPShader->LoadFromFile(filename, entry_point, "ps", "ps_5_0");
			//hr = mPShader->defaultShader("PShader", "ps", "ps_5_0");
			if (FAILED(hr))
			{
				printf("Failed compiling particle shader %08X\n", hr);
				return nullptr;
			}
			return mPShader;
		}		
	}

	GPUProgram* D3D11Renderer::addMaterial(unsigned char* buf, int buflen, char* shader_type, char* entry_point)
	{
		if (!strcmp(shader_type, "vs")) {
			mVShader = new D3D11HLSL(m_device, m_device_context);
			HRESULT hr = mVShader->LoadFromBuffer(buf, buflen, entry_point, "vs", "vs_5_0");
			//HRESULT hr = mVShader->defaultShader("VShader", "vs", "vs_5_0");
			if (FAILED(hr))
			{
				printf("Failed compiling vertex shader %08X\n", hr);
				return nullptr;
			}
			return mVShader;
		}
		else {
			mPShader = new D3D11HLSL(m_device, m_device_context);
			HRESULT hr = mPShader->LoadFromBuffer(buf, buflen, entry_point, "ps", "ps_5_0");
			//hr = mPShader->defaultShader("PShader", "ps", "ps_5_0");
			if (FAILED(hr))
			{
				printf("Failed compiling particle shader %08X\n", hr);
				return nullptr;
			}
			return mPShader;
		}
	}

	void D3D11Renderer::setBlendState(BlendState* state, float* blend_factor, unsigned int mask)
	{
		m_device_context->OMSetBlendState(static_cast<D3D11BlendState*>(state)->getBlendState(), blend_factor, mask);
	}

	PipelineStateObject* D3D11Renderer::createPipelineState(PipelineStateObjectDesc* desc)
	{
		D3D11PipelineStateObject* pso = new D3D11PipelineStateObject(this, desc);
		return pso;
	}

	void D3D11Renderer::setPipelineState(PipelineStateObject* pso)
	{
		if (m_current_pso != pso)
		{
			m_current_pso = pso;
			pso->SetState();
		}
	}

	void D3D11Renderer::setVertexShader(GPUProgram* material)
	{
		mVShader = static_cast<D3D11HLSL*>(material);		
		m_device_context->VSSetShader(mVShader->getVertexShader(), 0, 0);
	}

	void D3D11Renderer::setPixelShader(GPUProgram* material)
	{
		mPShader = static_cast<D3D11HLSL*>(material);
		m_device_context->PSSetShader(mPShader->getPixelShader(), 0, 0);
		
	}

	StencilState* D3D11Renderer::getStencilState(std::string name)
	{
		auto state = m_stencil_states.find(name);
		if (state != m_stencil_states.end()) {
			return state->second.get();
		}
		else {
			return nullptr;
		}
	}

	StencilState* D3D11Renderer::createStencilState(std::string name, DepthStencilStateDesc& desc)
	{
		D3D11StencilState* state = new D3D11StencilState(m_device, m_device_context, &desc);
		if (state) {
			m_stencil_states[name].reset(state);
			return m_stencil_states[name].get();
		}
		return nullptr;
	}

	void D3D11Renderer::setTexture(Texture* texture, int slot)
	{
		if (!slot) {
			if (m_current_texture != texture)
			{
				m_current_texture = texture;
				ID3D11ShaderResourceView* tex_view = ((Hippios::D3D11Texture*)texture)->get();
				m_device_context->PSSetShaderResources(0, 1, &tex_view);
			}
		}
		else {
			ID3D11ShaderResourceView* tex_view = ((Hippios::D3D11Texture*)texture)->get();
			m_device_context->PSSetShaderResources(slot, 1, &tex_view);
		}
	}

	void D3D11Renderer::draw(int size, int start_vtx)
	{
		m_device_context->Draw(size, start_vtx);
	}

	void D3D11Renderer::drawIndexed(int size, int start_idx, int start_vtx)
	{
		m_device_context->DrawIndexed(size, start_idx, start_vtx);
	}

	GraphicsBuffer* D3D11Renderer::initGPUBuffer(size_t size)
	{
		// setup and create a buffer for gpu params
		
		D3D11_BUFFER_DESC cbbd;
		ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

		cbbd.Usage = D3D11_USAGE_DEFAULT;
		// make sure size is a multiple of 16
		if (size % 16) {
			size_t base = size % 16;
			size = (base + 1) * 16;
		}
		cbbd.ByteWidth = (UINT)size;
		cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbbd.CPUAccessFlags = 0;
		cbbd.MiscFlags = 0;

		ID3D11Buffer* cb_buffer;
		HRESULT hr = m_device->CreateBuffer(&cbbd, NULL, &cb_buffer);
		D3D11GraphicsBuffer* buf = new D3D11GraphicsBuffer(m_device, m_device_context, cb_buffer);
		return buf;
	}
	
	void D3D11Renderer::bindGPUBuffer(GraphicsBuffer* buffer, int slot, char* stage)
	{
		ID3D11Buffer* cb_buffer = static_cast<D3D11GraphicsBuffer*>(buffer)->get_buffer();
		if (!strcmp(stage, "VS"))
			m_device_context->VSSetConstantBuffers(slot, 1, &cb_buffer);
		else if (!strcmp(stage, "PS"))
			m_device_context->PSSetConstantBuffers(slot, 1, &cb_buffer);
	}
}