#include "D3D11StencilState.h"
#include <stdio.h>
#include <Psio/Log.h>

namespace Hippios {

    D3D11StencilState::D3D11StencilState(ID3D11Device* device, ID3D11DeviceContext* device_context, DepthStencilStateDesc* desc) :
        m_depth_stencil_state(nullptr),
        m_device(device),
        m_device_context(device_context)
    {
        ZeroMemory(&m_depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        if (desc) {

            m_depth_stencil_desc.DepthEnable = desc->depth_enable;
            m_depth_stencil_desc.DepthWriteMask = (desc->depth_write_mask == DepthWriteMask::DEPTH_WRITE_ZERO) ? D3D11_DEPTH_WRITE_MASK_ZERO : D3D11_DEPTH_WRITE_MASK_ALL;
            m_depth_stencil_desc.DepthFunc = func(desc->depth_func);
            m_depth_stencil_desc.StencilEnable = desc->stencil_enable;
            m_depth_stencil_desc.StencilReadMask = desc->stencil_read_mask;
            m_depth_stencil_desc.StencilWriteMask = desc->stencil_write_mask;
            m_depth_stencil_desc.FrontFace.StencilFunc = func(desc->front_face.stencil_func);
            m_depth_stencil_desc.BackFace.StencilFunc = func(desc->back_face.stencil_func);
            m_depth_stencil_desc.FrontFace.StencilDepthFailOp = op(desc->front_face.depth_fail_op);
            m_depth_stencil_desc.BackFace.StencilDepthFailOp = op(desc->back_face.depth_fail_op);
            m_depth_stencil_desc.FrontFace.StencilPassOp = op(desc->front_face.pass_op);
            m_depth_stencil_desc.BackFace.StencilPassOp = op(desc->back_face.pass_op);
            m_depth_stencil_desc.FrontFace.StencilFailOp = op(desc->front_face.depth_fail_op);
            m_depth_stencil_desc.BackFace.StencilFailOp = op(desc->back_face.depth_fail_op);

            HRESULT result = m_device->CreateDepthStencilState(&m_depth_stencil_desc, &m_depth_stencil_state);
            if (FAILED(result))
                LOG(LOG_ERROR) << "D3D11StencilState: Could not create stencil state";
        }
        else
			// Create a default state
			m_depth_stencil_state = nullptr;
    }

    D3D11StencilState::~D3D11StencilState()
    {
        if (m_depth_stencil_state)
            m_depth_stencil_state->Release();
    }


    void D3D11StencilState::initialize()
    {
        if (m_depth_stencil_state)
            return;
        HRESULT result = m_device->CreateDepthStencilState(&m_depth_stencil_desc, &m_depth_stencil_state);
        if (FAILED(result))
            LOG(LOG_ERROR) << "D3D11StencilState: Could not create stencil state";
    }

    void D3D11StencilState::setState(unsigned int ref)
    {
        m_device_context->OMSetDepthStencilState(m_depth_stencil_state, ref);
    }

    D3D11_STENCIL_OP D3D11StencilState::op(StencilOp op)
    {
        switch (op) {
		case StencilOp::STENCIL_KEEP:
			return D3D11_STENCIL_OP_KEEP;
		case StencilOp::STENCIL_ZERO:
			return D3D11_STENCIL_OP_ZERO;
		case StencilOp::STENCIL_REPLACE:
			return D3D11_STENCIL_OP_REPLACE;
        case StencilOp::STENCIL_INCR_SAT:
            return D3D11_STENCIL_OP_INCR_SAT;
        case StencilOp::STENCIL_DECR_SAT:
            return D3D11_STENCIL_OP_DECR_SAT;
        case StencilOp::STENCIL_INVERT:
            return D3D11_STENCIL_OP_INVERT;
        case StencilOp::STENCIL_INCR:
			return D3D11_STENCIL_OP_INCR;
		case StencilOp::STENCIL_DECR:
			return D3D11_STENCIL_OP_DECR;		
		default:
			return D3D11_STENCIL_OP_KEEP;
        }
    }

    D3D11_COMPARISON_FUNC D3D11StencilState::func(ComparisonFunction func)
    {
		switch (func)
		{
		case ComparisonFunction::COMPARE_ALWAYS:
			return D3D11_COMPARISON_ALWAYS;
		case ComparisonFunction::COMPARE_EQUAL:
			return D3D11_COMPARISON_EQUAL;
		case ComparisonFunction::COMPARE_GREATER:
			return D3D11_COMPARISON_GREATER;
		case ComparisonFunction::COMPARE_GREATER_EQUAL:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case ComparisonFunction::COMPARE_LESS:
			return D3D11_COMPARISON_LESS;
		case ComparisonFunction::COMPARE_LESS_EQUAL:
			return D3D11_COMPARISON_LESS_EQUAL;
		case ComparisonFunction::COMPARE_NOT_EQUAL:
			return D3D11_COMPARISON_NOT_EQUAL;
		default:
			return D3D11_COMPARISON_ALWAYS;
		}
    }
}