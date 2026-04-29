/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef HIPPIOS_D3D11_PIPELINESTATE_H
#define HIPPIOS_D3D11_PIPELINESTATE_H

#include <Hippios/PipelineState.h>
#include <Hippios/Renderer.h>
#include "D3D11StencilState.h"
#include "D3D11BlendState.h"
#include <memory>
#include "D3D11Renderer.h"
#include "D3D11ShaderProgram.h"

namespace hippios
{
	class D3D11PipelineStateObject : public PipelineStateObject
	{
	public:
		D3D11PipelineStateObject(Renderer* renderer, const PipelineStateObjectDesc& desc);
		~D3D11PipelineStateObject();

		void SetState() override;
	
	protected:
		D3D11StencilState* m_stencil_state;
		std::unique_ptr<D3D11BlendState> m_blend_state;
		D3D11Renderer* m_renderer;
		D3D11ShaderProgram* m_shader_program;
		D3D_PRIMITIVE_TOPOLOGY m_topology;
		CullMode m_cull_mode;
		ID3D11RasterizerState* m_raster_state;
		ID3D11InputLayout* m_input_layout;
	};
}
#endif // !HIPPIOS_D3D11_PIPELINESTATE_H
