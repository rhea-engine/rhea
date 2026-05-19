/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "D3D11PipelineState.h"
#include "D3D11Renderer.h"
#include "D3D11ShaderProgram.h"
#include <cassert>
#include <Psio/Log.h>

namespace hippios
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

	D3D11PipelineStateObject::D3D11PipelineStateObject(Renderer* renderer, const PipelineStateObjectDesc& desc)
	{
		static int _internal_counter = 0;
		m_renderer = (D3D11Renderer*)renderer;

		// initialize depth stencil state
		// Look up by name first. If found, verify that the GPU-relevant fields match.
		// If they don't match (e.g. overrides changed depth_enable), create a new
		// uniquely-named state so we don't corrupt the original preset.
		m_stencil_state = static_cast<D3D11StencilState*>(
			m_renderer->getStencilState(desc.depthStencilState.name));

		if (m_stencil_state) {
			// Existing state found – does it match what we need?
			if (!m_stencil_state->getDesc().matches(desc.depthStencilState)) {
				// Overrides differ from the stored preset → create a new variant
				std::string uniqueName = desc.depthStencilState.name
					+ "_pso_" + std::to_string(_internal_counter++);
				m_stencil_state = static_cast<D3D11StencilState*>(
					m_renderer->createStencilState(uniqueName, desc.depthStencilState));
			}
		}
		else {
			// No state with this name exists yet → create it
			m_stencil_state = static_cast<D3D11StencilState*>(
				m_renderer->createStencilState(desc.depthStencilState.name, desc.depthStencilState));
		}
		

		// initialize rasterizer state


		// initialize blend state
		m_blend_state = std::make_unique<D3D11BlendState>(m_renderer->get_device(), desc.blendState);

		// initialize input layout

		// initialize primitive topology		
		m_shader_program = static_cast<D3D11ShaderProgram*>(desc.shaderProgram);

		m_cull_mode = desc.rasterizerState.cull_mode;

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
		if (desc.rasterizerState.fill_mode == FillMode::SOLID) 
			cmdesc.FillMode = D3D11_FILL_SOLID;
		else if (desc.rasterizerState.fill_mode == FillMode::WIREFRAME) {
			cmdesc.FillMode = D3D11_FILL_WIREFRAME;
			set_raster = true;
		}

		// always enable scissor test for now, as it's needed for UI rendering and doesn't have a significant performance impact when not used
		cmdesc.ScissorEnable = TRUE;

		if (set_raster) {
			m_renderer->get_device()->CreateRasterizerState(&cmdesc, &m_raster_state);
		}
		else {
			m_raster_state = nullptr;
		}

		switch (desc.primitiveTopology) {
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

		// vertex input layout
		D3D11_INPUT_ELEMENT_DESC* ied;
		ied = new D3D11_INPUT_ELEMENT_DESC[desc.inputLayout.num_elements];
		assert(desc.inputLayout.num_elements > 0);
		for (int i = 0; i < desc.inputLayout.num_elements; i++) {
			const auto& elem = desc.inputLayout.elements[i];

			const InputBindingDesc* bindingDesc = nullptr;
			for (int b = 0; b < desc.inputLayout.num_bindings; ++b)
			{
				if (desc.inputLayout.bindings[b].inputSlot == elem.input_slot)
				{
					bindingDesc = &desc.inputLayout.bindings[b];
					break;
				}
			}

			ied[i].SemanticName = elem.semantic_name;
			ied[i].SemanticIndex = elem.semantic_index;
			ied[i].Format = D3D11RendererGetFormat(elem.format);
			ied[i].InputSlot = elem.input_slot;
			ied[i].AlignedByteOffset = elem.aligned_byte_offset;

			if (bindingDesc && bindingDesc->instanceStepRate > 0)
			{
				ied[i].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
				ied[i].InstanceDataStepRate = bindingDesc->instanceStepRate;
			}
			else
			{
				ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				ied[i].InstanceDataStepRate = 0;
			}
		}
		// use the vertex blob from D3D11ShaderProgram for input layout creation
		ID3DBlob* vs_blob = m_shader_program->getVertexBlob();
		if (!vs_blob) {
			LOG(LOG_ERROR) << "D3D11PipelineStateObject: no vertex shader found";
			delete[] ied;
			m_input_layout = nullptr;
			return;
		}
		HRESULT HR = m_renderer->get_device()->CreateInputLayout(
			ied,
			desc.inputLayout.num_elements,
			vs_blob->GetBufferPointer(),
			vs_blob->GetBufferSize(),
			&m_input_layout);
		if (FAILED(HR)) {
			LOG(LOG_DEBUG) << "CreateInputLayout: HRESULT: 0x" << std::hex << HR;
			LOG(LOG_ERROR) << "D3D11PipelineStateObject: Could not create input layout for shader - expect shader to fail.";
		}
		delete[] ied;
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

		if (!m_input_layout)
			return;
		
		m_renderer->setStencilState(m_stencil_state, 1);
		m_renderer->setRasterizerState(m_raster_state);
		m_renderer->setBlendState(m_blend_state.get(), blend_factor, blend_mask);
		m_renderer->get_device_context()->IASetInputLayout(m_input_layout);
		m_renderer->get_device_context()->IASetPrimitiveTopology(m_topology);
		m_renderer->get_device_context()->VSSetShader(m_shader_program->getVertexShader(), nullptr, 0);
		m_renderer->get_device_context()->PSSetShader(m_shader_program->getPixelShader(), nullptr, 0);
	}
}
