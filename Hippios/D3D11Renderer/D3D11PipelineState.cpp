#include "D3D11PipelineState.h"
#include "D3D11Renderer.h"

namespace Hippios
{
	D3D11PipelineStateObject::D3D11PipelineStateObject(Renderer* renderer, PipelineStateObjectDesc* desc)
	{
		m_renderer = (D3D11Renderer*)renderer;

		// initialize dpeth stencil state
		m_stencil_state = static_cast<D3D11StencilState*>(m_renderer->getStencilState(desc->depthStencilState.name));		
		if (!m_stencil_state)
			m_stencil_state = static_cast<D3D11StencilState*>(m_renderer->createStencilState(desc->depthStencilState.name, desc->depthStencilState));
		
		// initialize rasterizer state


		// initialize blend state
		m_blend_state = std::make_unique<D3D11BlendState>(m_renderer->get_device(), desc->blendState);

		// initialize input layout

		// initialize primitive topology
		m_topology = desc->vertexLayout.topology;
		m_vertex_shader = desc->shaderProgram.vertex_shader;
		m_pixel_shader = desc->shaderProgram.pixel_shader;

		m_cull_mode = desc->cull_mode;

		if (m_cull_mode == CullMode::NONE) {
			D3D11_RASTERIZER_DESC cmdesc;
			ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
			cmdesc.FillMode = D3D11_FILL_SOLID;
			cmdesc.CullMode = D3D11_CULL_NONE;
			m_renderer->get_device()->CreateRasterizerState(&cmdesc, &m_raster_state);
		}
		else {
			m_raster_state = nullptr;
		}

		 
//		if (desc->topology == 4)
//			d3d11Renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		else
//			d3d11Renderer->get_device_context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	D3D11PipelineStateObject::~D3D11PipelineStateObject()
	{
		if (m_raster_state)
			m_raster_state->Release();
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
		m_renderer->setVertexShader(m_vertex_shader, m_topology);
		m_renderer->setPixelShader(m_pixel_shader);
	}
}
