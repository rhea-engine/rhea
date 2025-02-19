#include "D3D11PipelineState.h"
#include "D3D11Renderer.h"
#include "D3D11HLSL.h"
#include <cassert>
#include <Psio/Log.h>

namespace Hippios
{
	DXGI_FORMAT D3D11RendererGetFormat(InputLayoutFormat format)
	{
		switch (format) {
		case InputLayoutFormat::R32G32B32A32_FLOAT:
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case InputLayoutFormat::R32G32B32_FLOAT:
			return DXGI_FORMAT_R32G32B32_FLOAT;
		case InputLayoutFormat::R32G32_FLOAT:
			return DXGI_FORMAT_R32G32_FLOAT;
		case InputLayoutFormat::R32_FLOAT:
			return DXGI_FORMAT_R32_FLOAT;
		case InputLayoutFormat::R32G32B32A32_UINT:
			return DXGI_FORMAT_R32G32B32A32_UINT;
		case InputLayoutFormat::R32_UINT:
			return DXGI_FORMAT_R32_UINT;
		case InputLayoutFormat::R32_SINT:
			return DXGI_FORMAT_R32_SINT;
		case InputLayoutFormat::R16G16B16A16_FLOAT:
			return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case InputLayoutFormat::R16G16_FLOAT:
			return DXGI_FORMAT_R16G16_FLOAT;
		case InputLayoutFormat::R16_FLOAT:
			return DXGI_FORMAT_R16_FLOAT;
		case InputLayoutFormat::R16G16B16A16_UINT:
			return DXGI_FORMAT_R16G16B16A16_UINT;
		case InputLayoutFormat::R16G16_UINT:
			return DXGI_FORMAT_R16G16_UINT;
		case InputLayoutFormat::R16_UINT:
			return DXGI_FORMAT_R16_UINT;
		case InputLayoutFormat::R16G16B16A16_SINT:
			return DXGI_FORMAT_R16G16B16A16_SINT;
		case InputLayoutFormat::R16G16_SINT:
			return DXGI_FORMAT_R16G16_SINT;
		case InputLayoutFormat::R16_SINT:
			return DXGI_FORMAT_R16_SINT;
		case InputLayoutFormat::R8G8B8A8_UNORM:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case InputLayoutFormat::R8G8_UNORM:
			return DXGI_FORMAT_R8G8_UNORM;
		case InputLayoutFormat::R8_UNORM:
			return DXGI_FORMAT_R8_UNORM;
		case InputLayoutFormat::R8_UINT:
			return DXGI_FORMAT_R8_UINT;
		case InputLayoutFormat::R8_SINT:
			return DXGI_FORMAT_R8_SINT;
		default:
			LOG(LOG_ERROR) << "Create Shader (D3D11PipelineStateObject): Warning: Unknown input element type - expect shader to fail.";
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	D3D11PipelineStateObject::D3D11PipelineStateObject(Renderer* renderer, PipelineStateObjectDesc* desc)
	{
		m_renderer = (D3D11Renderer*)renderer;

		// initialize dpeth stencil state
		m_stencil_state = static_cast<D3D11StencilState*>(m_renderer->getStencilState(desc->depthStencilState.name));		
		if (!m_stencil_state)
			m_stencil_state = static_cast<D3D11StencilState*>(m_renderer->createStencilState(desc->depthStencilState.name, &desc->depthStencilState));
		
		// initialize rasterizer state


		// initialize blend state
		m_blend_state = std::make_unique<D3D11BlendState>(m_renderer->get_device(), desc->blendState);

		// initialize input layout

		// initialize primitive topology		
		m_vertex_shader = desc->shaderProgram.vertex_shader;
		m_pixel_shader = desc->shaderProgram.pixel_shader;

		m_cull_mode = desc->rasterizerState.cull_mode;

		D3D11_RASTERIZER_DESC cmdesc = {};
		bool set_raster = false;
		if (m_cull_mode == CullMode::NONE) {
			cmdesc.CullMode = D3D11_CULL_NONE;
			set_raster = true;
		}
		else if (m_cull_mode == CullMode::FRONT)
		{
			cmdesc.CullMode = D3D11_CULL_FRONT;
			set_raster = true;
		}
		if (desc->rasterizerState.fill_mode == FillMode::SOLID) 
			cmdesc.FillMode = D3D11_FILL_SOLID;
		else if (desc->rasterizerState.fill_mode == FillMode::WIREFRAME) {
			cmdesc.FillMode = D3D11_FILL_WIREFRAME;
			set_raster = true;
		}
		if (set_raster) {
			m_renderer->get_device()->CreateRasterizerState(&cmdesc, &m_raster_state);
		}
		else {
			m_raster_state = nullptr;
		}

		// vertex input layout
		D3D11_INPUT_ELEMENT_DESC* ied;
		ied = new D3D11_INPUT_ELEMENT_DESC[desc->inputLayout.num_elements];
		assert(desc->inputLayout.num_elements > 0);
		for (int i = 0; i < desc->inputLayout.num_elements; i++) {
			ied[i].SemanticName = desc->inputLayout.elements[i].semantic_name;
			ied[i].SemanticIndex = 0;
			ied[i].Format = D3D11RendererGetFormat(desc->inputLayout.elements[i].format);
			ied[i].InputSlot = 0;
			ied[i].AlignedByteOffset = desc->inputLayout.elements[i].aligned_byte_offset;
			ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			ied[i].InstanceDataStepRate = 0;
		}
		HRESULT HR = m_renderer->get_device()->CreateInputLayout(ied, desc->inputLayout.num_elements, ((D3D11HLSL*)m_vertex_shader)->getBuffer(), ((D3D11HLSL*)m_vertex_shader)->getBufferSize(), &m_input_layout);
		if (FAILED(HR)) {
			LOG(LOG_DEBUG) << "CreateInputLayout: HRESULT: 0x" << std::hex << HR;
			LOG(LOG_ERROR) << "D3D11PipelineStateObject: Could not create input layout for shader - expect shader to fail.";
		}
		delete[] ied;

		switch (desc->vertexLayout) {
		case PrimitiveTopology::LineList:
			m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			break;
		case PrimitiveTopology::TriangleList:
			m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			break;
		case PrimitiveTopology::TriangleStrip:
			m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			break;
		default:
			m_topology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
			break;
		}
	}

	D3D11PipelineStateObject::~D3D11PipelineStateObject()
	{
		if (m_raster_state)
			m_raster_state->Release();
		if (m_input_layout)
			m_input_layout->Release();
	}

	void D3D11PipelineStateObject::SetState()
	{
		float blend_factor[4];

		// Setup the blend factor.
		blend_factor[0] = 0.0f;
		blend_factor[1] = 0.0f;
		blend_factor[2] = 0.0f;
		blend_factor[3] = 0.0f;

		unsigned int blend_mask = 0xffffffff;
		
		m_renderer->setStencilState(m_stencil_state, 1);
		m_renderer->setRasterizerState(m_raster_state);
		m_renderer->setBlendState(m_blend_state.get(), blend_factor, blend_mask);
		m_renderer->get_device_context()->IASetInputLayout(m_input_layout);
		m_renderer->get_device_context()->IASetPrimitiveTopology(m_topology);
		m_renderer->setVertexShader(m_vertex_shader);
		m_renderer->setPixelShader(m_pixel_shader);
	}
}
