/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "D3D11BlendState.h"
#include <Psio/Log.h>

namespace hippios
{
	D3D11BlendState::D3D11BlendState(ID3D11Device* device, const BlendStateDesc& desc)
	{
		// create blend state
		D3D11_BLEND_DESC blend_state_description;
		ZeroMemory(&blend_state_description, sizeof(D3D11_BLEND_DESC));

		blend_state_description.AlphaToCoverageEnable = desc.alphaToCoverageEnable;
		blend_state_description.IndependentBlendEnable = desc.independentBlendEnable;
		for (int i = 0; i < 8; i++)
		{
			blend_state_description.RenderTarget[i].RenderTargetWriteMask = desc.renderTarget[i].render_target_write_mask;
			blend_state_description.RenderTarget[i].BlendEnable = desc.renderTarget[i].blend_enable;
			blend_state_description.RenderTarget[i].SrcBlend = blend(desc.renderTarget[i].src_blend);
			blend_state_description.RenderTarget[i].DestBlend = blend(desc.renderTarget[i].dest_blend);
			blend_state_description.RenderTarget[i].BlendOp = blendOp(desc.renderTarget[i].blend_op);
			blend_state_description.RenderTarget[i].SrcBlendAlpha = blend(desc.renderTarget[i].src_blend_alpha);
			blend_state_description.RenderTarget[i].DestBlendAlpha = blend(desc.renderTarget[i].dest_blend_alpha);
			blend_state_description.RenderTarget[i].BlendOpAlpha = blendOp(desc.renderTarget[i].blend_op_alpha);
		}

		HRESULT hr = device->CreateBlendState(&blend_state_description, &m_state);
		if (FAILED(hr))
		{
			LOG(LOG_ERROR) << "D3D11BlendState: Could not create blend state: " << hr;
		}
	}

	D3D11BlendState::~D3D11BlendState()
	{
		if (m_state)
			m_state->Release();
	}

	D3D11_BLEND D3D11BlendState::blend(BlendFactor factor)
	{
		switch (factor) {
		case BlendFactor::BLEND_ONE:
			return D3D11_BLEND_ONE;
		case BlendFactor::BLEND_ZERO:
			return D3D11_BLEND_ZERO;
		case BlendFactor::BLEND_SRC_COLOR:
			return D3D11_BLEND_SRC_COLOR;
		case BlendFactor::BLEND_INV_SRC_COLOR:
			return D3D11_BLEND_INV_SRC_COLOR;
		case BlendFactor::BLEND_SRC_ALPHA:
			return D3D11_BLEND_SRC_ALPHA;
		case BlendFactor::BLEND_INV_SRC_ALPHA:
			return D3D11_BLEND_INV_SRC_ALPHA;		
		case BlendFactor::BLEND_SRC_ALPHA_SAT:
			return D3D11_BLEND_SRC_ALPHA_SAT;
		case BlendFactor::BLEND_BLEND_FACTOR:
			return D3D11_BLEND_BLEND_FACTOR;
		case BlendFactor::BLEND_INV_BLEND_FACTOR:
			return D3D11_BLEND_INV_BLEND_FACTOR;
		case BlendFactor::BLEND_SRC1_COLOR:
			return D3D11_BLEND_SRC1_COLOR;
		case BlendFactor::BLEND_INV_SRC1_COLOR:
			return D3D11_BLEND_INV_SRC1_COLOR;
		case BlendFactor::BLEND_SRC1_ALPHA:
			return D3D11_BLEND_SRC1_ALPHA;
		case BlendFactor::BLEND_INV_SRC1_ALPHA:
			return D3D11_BLEND_INV_SRC1_ALPHA;
		default:
			return D3D11_BLEND_ONE;
		}	
	}

	D3D11_BLEND_OP D3D11BlendState::blendOp(BlendOp op)
	{
		switch (op) {
		case BlendOp::BLEND_ADD:
			return D3D11_BLEND_OP_ADD;
		case BlendOp::BLEND_SUBTRACT:
			return D3D11_BLEND_OP_SUBTRACT;
		case BlendOp::BLEND_REVERSE_SUBTRACT:
			return D3D11_BLEND_OP_REV_SUBTRACT;
		case BlendOp::BLEND_MIN:
			return D3D11_BLEND_OP_MIN;
		case BlendOp::BLEND_MAX:
			return D3D11_BLEND_OP_MAX;
		default:
			return D3D11_BLEND_OP_ADD;
		}
	}
}