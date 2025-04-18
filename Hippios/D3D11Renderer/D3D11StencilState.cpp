#include "D3D11StencilState.h"
#include <stdio.h>
#include <Psio/Log.h>

#include "D3D11BaseTypes.h"

namespace Hippios {

    // constants
    std::unordered_map<StencilOp, D3D11_STENCIL_OP> _stencil_ops = {
        { StencilOp::STENCIL_KEEP, D3D11_STENCIL_OP_KEEP},
        { StencilOp::STENCIL_ZERO, D3D11_STENCIL_OP_ZERO},
        { StencilOp::STENCIL_REPLACE, D3D11_STENCIL_OP_REPLACE},
        { StencilOp::STENCIL_INCR_SAT, D3D11_STENCIL_OP_INCR_SAT},
        { StencilOp::STENCIL_DECR_SAT, D3D11_STENCIL_OP_DECR_SAT},
        { StencilOp::STENCIL_INVERT, D3D11_STENCIL_OP_INVERT},
        { StencilOp::STENCIL_INCR, D3D11_STENCIL_OP_INCR},
        { StencilOp::STENCIL_DECR, D3D11_STENCIL_OP_DECR}
    };

    D3D11StencilState::D3D11StencilState(ID3D11Device* device, ID3D11DeviceContext* device_context, DepthStencilStateDesc* desc) :
        m_depth_stencil_state(nullptr),
        m_device(device),
        m_device_context(device_context)
    {
        ZeroMemory(&m_depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        if (desc) {

            m_depth_stencil_desc.DepthEnable = desc->depth_enable;
            m_depth_stencil_desc.DepthWriteMask = (desc->depth_write_mask == DepthWriteMask::DEPTH_WRITE_ZERO) ? D3D11_DEPTH_WRITE_MASK_ZERO : D3D11_DEPTH_WRITE_MASK_ALL;
            m_depth_stencil_desc.DepthFunc = _comparison_functions[desc->depth_func];
            m_depth_stencil_desc.StencilEnable = desc->stencil_enable;
            m_depth_stencil_desc.StencilReadMask = desc->stencil_read_mask;
            m_depth_stencil_desc.StencilWriteMask = desc->stencil_write_mask;
            m_depth_stencil_desc.FrontFace.StencilFunc = _comparison_functions[desc->front_face.stencil_func];
            m_depth_stencil_desc.BackFace.StencilFunc = _comparison_functions[desc->back_face.stencil_func];
            m_depth_stencil_desc.FrontFace.StencilDepthFailOp = _stencil_ops[desc->front_face.depth_fail_op];
            m_depth_stencil_desc.BackFace.StencilDepthFailOp = _stencil_ops[desc->back_face.depth_fail_op];
            m_depth_stencil_desc.FrontFace.StencilPassOp = _stencil_ops[desc->front_face.pass_op];
            m_depth_stencil_desc.BackFace.StencilPassOp = _stencil_ops[desc->back_face.pass_op];
            m_depth_stencil_desc.FrontFace.StencilFailOp = _stencil_ops[desc->front_face.depth_fail_op];
            m_depth_stencil_desc.BackFace.StencilFailOp = _stencil_ops[desc->back_face.depth_fail_op];

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
}