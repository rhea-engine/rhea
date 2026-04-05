#include "D3D11Renderer.h"
#include "D3D11Renderer.h"
#include "D3D11Renderer.h"
#include <d3d11shader.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#if defined(DEBUG) || defined(_DEBUG)
#include <dxgi1_3.h>
#include <dxgidebug.h>
#endif
#include <iostream>
#include <assert.h>

#include "D3D11Renderer.h"
#include "D3D11BaseTypes.h"
#include "D3D11VertexBuffer.h"
#include "D3D11IndexBuffer.h"
#include "D3D11GraphicsBuffer.h"
#include <Hippios/Texture.h>
#include "D3D11Texture.h"
#include "D3D11StencilState.h"
#include "D3D11PipelineState.h"
#include "D3D11SwapChain.h"
#include "D3D11RenderTarget.h"
#include "D3D11BlendState.h"
#include "D3D11ShaderProgram.h"
#include "D3D11SamplerState.h"
#include <Psio/Log.h>

#pragma comment (lib, "d3d11.lib")

namespace hippios {

	// constant mappings
	std::unordered_map<SamplerDesc::Filter, D3D11_FILTER> _sampler_filters = {
		{SamplerDesc::Filter::LINEAR, D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR},
		{SamplerDesc::Filter::MIP_POINT, D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT}
	};

	std::unordered_map<SamplerDesc::AddressMode, D3D11_TEXTURE_ADDRESS_MODE> _address_modes = {
		{SamplerDesc::AddressMode::WRAP, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP},
		{SamplerDesc::AddressMode::CLAMP, D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP}
	};

	D3D11Renderer::D3D11Renderer() :
		D3D11Renderer(nullptr, nullptr)
	{
	}

	const char* FeatureLevelToString(D3D_FEATURE_LEVEL fl)
	{
		switch (fl)
		{
		case D3D_FEATURE_LEVEL_11_1: return "11.1";
		case D3D_FEATURE_LEVEL_11_0: return "11.0";
		case D3D_FEATURE_LEVEL_10_1: return "10.1";
		case D3D_FEATURE_LEVEL_10_0: return "10.0";
		case D3D_FEATURE_LEVEL_9_3:  return "9.3";
		case D3D_FEATURE_LEVEL_9_2:  return "9.2";
		case D3D_FEATURE_LEVEL_9_1:  return "9.1";
		default: return "Unknown";
		}
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
		m_current_raster_state = nullptr;
	}
	
	HRESULT D3D11Renderer::createDeviceAndSwapChain(D3D_DRIVER_TYPE driverType, D3D_FEATURE_LEVEL minFL, D3D_FEATURE_LEVEL maxFL, D3D_FEATURE_LEVEL* pFeatureLevel, NativeWindow* win, IDXGISwapChain** swap_chain)
	{
		IDXGIAdapter* pAdapter = NULL;

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

		DXGI_SWAP_CHAIN_DESC desc;
		ZeroMemory(&desc, sizeof(DXGI_SWAP_CHAIN_DESC));
				
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BufferDesc.Width = win->width;
		desc.BufferDesc.Height = win->height;
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
		desc.OutputWindow = (HWND)win->native_win_handle;
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
			swap_chain,
			&m_device,
			NULL,
			&m_device_context);

		if (FAILED(hr))
		{
			// Handle device interface creation failure if it occurs.
			// For example, reduce the feature level requirement, or fail over 
			// to WARP rendering.
			std::cout << "could not create d3d11 device";
			return hr;
		}

		return hr;
	}

	HRESULT D3D11Renderer::createDevice(
		D3D_DRIVER_TYPE driverType,
		D3D_FEATURE_LEVEL minFL,
		D3D_FEATURE_LEVEL maxFL,
		D3D_FEATURE_LEVEL* pFeatureLevel)
	{
		if (m_device && m_device_context) {
			if (pFeatureLevel)
				*pFeatureLevel = mFeatureLevel;
			return S_OK;
		}

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// Ordered highest -> lowest, because D3D11CreateDevice picks the first supported level.
		const D3D_FEATURE_LEVEL allLevels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		// Filter by requested min/max range.
		D3D_FEATURE_LEVEL requestedLevels[ARRAYSIZE(allLevels)];
		UINT requestedCount = 0;

		for (D3D_FEATURE_LEVEL level : allLevels) {
			if (level <= maxFL && level >= minFL) {
				requestedLevels[requestedCount++] = level;
			}
		}

		if (requestedCount == 0) {
			return E_INVALIDARG;
		}

		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		D3D_FEATURE_LEVEL createdFeatureLevel = D3D_FEATURE_LEVEL_9_1;

		HRESULT hr = D3D11CreateDevice(
			nullptr,                    // default adapter
			driverType,
			nullptr,                    // no software rasterizer module
			creationFlags,
			requestedLevels,
			requestedCount,
			D3D11_SDK_VERSION,
			&device,
			&createdFeatureLevel,
			&context);

		if (FAILED(hr)) {
			// Optional fallback to WARP if hardware device creation fails.
			if (driverType == D3D_DRIVER_TYPE_HARDWARE) {
				hr = D3D11CreateDevice(
					nullptr,
					D3D_DRIVER_TYPE_WARP,
					nullptr,
					creationFlags,
					requestedLevels,
					requestedCount,
					D3D11_SDK_VERSION,
					&device,
					&createdFeatureLevel,
					&context);
			}

			if (FAILED(hr)) {
				return hr;
			}
		}

		m_device = device;
		m_device_context = context;
		mDriverType = driverType;
		mFeatureLevel = createdFeatureLevel;
		mMinRequestedFeatureLevel = minFL;
		mMaxRequestedFeatureLevel = maxFL;

		if (pFeatureLevel)
			*pFeatureLevel = createdFeatureLevel;

		// log directx and feature level
		LOG(LOG_INFO) << "Direct3D 11 device created with feature level " << FeatureLevelToString(createdFeatureLevel);

#ifdef _DEBUG
		IDXGIDevice* dxgiDevice = nullptr;
		m_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);

		IDXGIAdapter* adapter = nullptr;
		dxgiDevice->GetAdapter(&adapter);

		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		// Convert WCHAR to char
		char name[128];
		size_t convertedChars = 0;
		wcstombs_s(&convertedChars, name, sizeof(name), desc.Description, _TRUNCATE);

		LOG(LOG_INFO) << "D3D11 Adapter: " << name;

		adapter->Release();
		dxgiDevice->Release();
#endif

		return S_OK;
	}

	void D3D11Renderer::bindBuffers(VertexBuffer* vb, IndexBuffer* ib)
	{
		if (m_last_vb != vb) {
			m_last_vb = static_cast<D3D11VertexBuffer*>(vb);
			m_last_vb->is_dirty = true;
		}

		if (m_last_vb && m_last_vb->is_dirty) {
			UINT stride = (UINT)m_last_vb->get_vertex_size();
			UINT offset = 0;
			ID3D11Buffer* buffer = m_last_vb->get_buffer();
			m_device_context->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
			m_last_vb->is_dirty = false;
		}

		if (m_last_ib != ib && ib) {
			m_last_ib = static_cast<D3D11IndexBuffer*>(ib);
			m_last_ib->is_dirty = true;
		}

		// set index buffer
		if (m_last_ib && m_last_ib->is_dirty) {
			ID3D11Buffer* buffer = m_last_ib->get_buffer();
			m_device_context->IASetIndexBuffer(buffer, m_last_ib->getIndexFormat(), 0);
			m_last_ib->is_dirty = false;
		}
	}

	bool D3D11Renderer::setActiveSwapChain(SwapChain* swap_chain)
	{
		if (!swap_chain)
			return false;

		// Direct3D 11 does not have a concept of "active swap chain", instead we set the render target to the back buffer of the swap chain
		return true;
	}

	void D3D11Renderer::clearPass(RenderTarget* render_target)
	{
		render_target->clearPass();
	}

	int D3D11Renderer::presentPass(SwapChain* swap_chain)
	{
		if (!swap_chain)
			return 0;
		IDXGISwapChain* pDXGISwapChain = static_cast<D3D11SwapChain*>(swap_chain)->getNativeSwapChain();
		if (!pDXGISwapChain)
			return 0;

		// switch the back buffer and the front buffer
		HRESULT hr = pDXGISwapChain->Present(0, 0);
		if (FAILED(hr)) {
			printf("Swapchain.Present failed\n");
			return -1;
		}
		return 0;
	}

	bool D3D11Renderer::getStencilStateDesc(const std::string& name, DepthStencilStateDesc& outDesc)
	{
		D3D11StencilState* state = static_cast<D3D11StencilState*>(getStencilState(name));
		if (state) {
			outDesc = state->getDesc();
			return true;
		}
		return false;
	}
	
	void D3D11Renderer::setStencilState(StencilState* state, int stencil_ref)
	{
		if (state != m_current_stencil_state) {
			m_current_stencil_state = static_cast<D3D11StencilState*>(state);
			m_current_stencil_state->setState(stencil_ref);
		}
	}

	SamplerState* D3D11Renderer::createSampler(const SamplerDesc& desc)
	{
		// Create default sampler states
		D3D11_SAMPLER_DESC sampDesc;

		// linear sampler
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = _sampler_filters[desc.filter];
		sampDesc.AddressU = _address_modes[desc.address_u];
		sampDesc.AddressV = _address_modes[desc.address_v];
		sampDesc.AddressW = _address_modes[desc.address_w];
		sampDesc.ComparisonFunc = _comparison_functions[desc.comparison_func];
		sampDesc.MinLOD = desc.min_lod;
		sampDesc.MaxLOD = desc.max_lod;

		// Create the Sample State
		ID3D11SamplerState* sampler_state;
		HRESULT HR = m_device->CreateSamplerState(&sampDesc, &sampler_state);

		if (FAILED(HR))
			return nullptr;

		auto* sampler = new D3D11SamplerState();
		sampler->state = sampler_state;
		return sampler;
	}

	void D3D11Renderer::setSampler(int slot, SamplerState* sampler, ShaderStage stage)
	{
		auto* d3dSampler = static_cast<D3D11SamplerState*>(sampler);
		ID3D11SamplerState* native = d3dSampler ? d3dSampler->state : nullptr;

		switch (stage)
		{
		case ShaderStage::Vertex:
			m_device_context->VSSetSamplers(slot, 1, &native);
			break;
		case ShaderStage::Fragment:
			m_device_context->PSSetSamplers(slot, 1, &native);
			break;
		default:
			break;
		}
	}

	void D3D11Renderer::setRasterizerState(ID3D11RasterizerState* state)
	{
		if (state != m_current_raster_state) {
			m_current_raster_state = state;
			m_device_context->RSSetState(m_current_raster_state);
		}
	}

	std::unique_ptr<VertexBuffer> D3D11Renderer::createVertexBuffer(size_t vertexSize, size_t numVertices, bool dynamic)
	{
		return std::make_unique<D3D11VertexBuffer>(m_device, m_device_context, vertexSize, numVertices, dynamic);
	}
	
	std::unique_ptr<IndexBuffer> D3D11Renderer::createIndexBuffer(size_t index_stride, size_t num_faces, bool dynamic)
	{
		return std::make_unique<D3D11IndexBuffer>(m_device, m_device_context, index_stride, num_faces, dynamic);
	}

	std::unique_ptr<GraphicsBuffer> D3D11Renderer::createGraphicsBuffer(size_t size_in_bytes)
	{
		return std::make_unique<D3D11GraphicsBuffer>(m_device, m_device_context, D3D11_BIND_CONSTANT_BUFFER, size_in_bytes, false, 0);
	}

	D3D11Renderer::~D3D11Renderer()
	{
		//m_pDXGISwapChain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode to allow D3D11 to close

		// close and release all existing COM objects

		// release input layouts
		if (m_device_context)
			m_device_context->IASetInputLayout(NULL);
		for (auto it = m_input_layouts.begin(); it != m_input_layouts.end(); ++it)
			(*it)->Release();

		if (m_device)
			m_device->Release();
		if (m_device_context)
			m_device_context->Release();
	}

	std::unique_ptr<RenderTarget> D3D11Renderer::createRenderTarget(const RenderTargetDesc& desc)
	{
		auto target = std::make_unique<D3D11RenderTarget>();
		if (target->create(this, desc))
			return target;
		else
			return nullptr;
	}

	std::unique_ptr<Texture> D3D11Renderer::createTexture(TextureDesc& desc)
	{
		return std::make_unique<D3D11Texture>(desc, this);
	}

	ShaderProgram* D3D11Renderer::createShader(const ShaderProgramDesc& desc)
	{
		auto* program = new D3D11ShaderProgram(m_device);

		if (!program->loadFromFiles(
			desc.vertexFile,
			desc.vertexEntry,
			desc.pixelFile,
			desc.pixelEntry))
		{
			delete program;
			return nullptr;
		}

		return program;
	}

	ShaderProgram* D3D11Renderer::createShader(const ShaderProgramSourceDesc& desc)
	{
		auto* program = new D3D11ShaderProgram(m_device);

		if (!program->loadFromSource(
			desc.vertexSource,
			desc.vertexSourceLength,
			desc.vertexEntry,
			desc.pixelSource,
			desc.pixelSourceLength,
			desc.pixelEntry))
		{
			delete program;
			return nullptr;
		}

		return program;
	}

	HRESULT D3D11Renderer::createSwapChainForWindow(NativeWindow* win, IDXGISwapChain** swap_chain)
	{
		if (!m_device || !win || !swap_chain)
			return E_INVALIDARG;

		*swap_chain = nullptr;

		Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
		HRESULT hr = m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgiDevice.GetAddressOf()));
		if (FAILED(hr))
			return hr;

		Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
		hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());
		if (FAILED(hr))
			return hr;

		Microsoft::WRL::ComPtr<IDXGIFactory> factory;
		hr = adapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(factory.GetAddressOf()));
		if (FAILED(hr))
			return hr;

		DXGI_SWAP_CHAIN_DESC desc = {};
		desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BufferDesc.Width = win->width;
		desc.BufferDesc.Height = win->height;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 1;
		desc.OutputWindow = (HWND)win->native_win_handle;
		desc.Windowed = TRUE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		return factory->CreateSwapChain(m_device, &desc, swap_chain);
	}

	std::unique_ptr<SwapChain> D3D11Renderer::createSwapChain(NativeWindow* win)
	{		
		if (!win)
			return nullptr;

		if (!m_device) {
			HRESULT hr = createDevice(mDriverType, mMinRequestedFeatureLevel, mMaxRequestedFeatureLevel, &mFeatureLevel);
			if (FAILED(hr))
				return nullptr;
		}

		IDXGISwapChain* d3_swap_chain = nullptr;
		HRESULT hr = createSwapChainForWindow(win, &d3_swap_chain);
		if (FAILED(hr))
			return nullptr;

		return std::make_unique<D3D11SwapChain>(d3_swap_chain);
	}

	void D3D11Renderer::setBlendState(BlendState* state, float* blend_factor, unsigned int mask)
	{
		m_device_context->OMSetBlendState(static_cast<D3D11BlendState*>(state)->getBlendState(), blend_factor, mask);
	}

	std::unique_ptr<PipelineStateObject> D3D11Renderer::createPipelineState(const PipelineStateObjectDesc& desc)
	{
		return std::make_unique<D3D11PipelineStateObject>(this, desc);
	}

	void D3D11Renderer::setPipelineState(PipelineStateObject* pso)
	{
		if (m_current_pso != pso)
		{
			m_current_pso = pso;
			pso->SetState();
		}
	}

	StencilState* D3D11Renderer::getStencilState(const std::string& name)
	{
		auto state = m_stencil_states.find(name);
		if (state != m_stencil_states.end()) {
			return state->second.get();
		}
		else {
			return nullptr;
		}
	}

	StencilState* D3D11Renderer::createStencilState(const std::string& name, const DepthStencilStateDesc& desc)
	{
		D3D11StencilState* state = new D3D11StencilState(m_device, m_device_context, desc);
		if (state) {
			m_stencil_states[name].reset(state);
			return m_stencil_states[name].get();
		}
		return nullptr;
	}

	void D3D11Renderer::setTexture(Texture* texture, int slot)
	{
		assert(texture);
		if (!texture)
			return;
		if (!slot) {
			if (m_current_texture != texture)
			{
				m_current_texture = texture;
				ID3D11ShaderResourceView* tex_view = ((hippios::D3D11Texture*)texture)->get();
				m_device_context->PSSetShaderResources(0, 1, &tex_view);
			}
		}
		else {
			ID3D11ShaderResourceView* tex_view = ((hippios::D3D11Texture*)texture)->get();
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
			size_t base = size / 16;
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
	
	void D3D11Renderer::bindGPUBuffer(GraphicsBuffer* buffer, int slot, ShaderStage stage)
	{
		ID3D11Buffer* cb_buffer = static_cast<D3D11GraphicsBuffer*>(buffer)->get_buffer();
		if (stage == ShaderStage::Vertex)
			m_device_context->VSSetConstantBuffers(slot, 1, &cb_buffer);
		else if (stage == ShaderStage::Fragment)
			m_device_context->PSSetConstantBuffers(slot, 1, &cb_buffer);
	}

	void D3D11Renderer::prepareMatricesForShader(math::Matrix4* dst, const math::Matrix4* src, size_t count) const
	{
		for (size_t i = 0; i < count; ++i)
			dst[i] = src[i];
	}

}