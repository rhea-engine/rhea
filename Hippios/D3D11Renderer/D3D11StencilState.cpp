#include "D3D11StencilState.h"
#include <stdio.h>

namespace Hippios {

    D3D11StencilState::D3D11StencilState(ID3D11Device* device, ID3D11DeviceContext* device_context) :
        m_depth_stencil_state(nullptr),
        m_device(device),
        m_device_context(device_context)
    {
        ZeroMemory(&m_depth_stencil_desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

        m_depth_stencil_desc.DepthEnable = FALSE;
        m_depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        m_depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
        m_depth_stencil_desc.StencilEnable = FALSE;
        m_depth_stencil_desc.StencilReadMask = 0x00;
        m_depth_stencil_desc.StencilWriteMask = 0x00;
        m_depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        m_depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        m_depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        m_depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        m_depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        m_depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        m_depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        m_depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    }

    D3D11StencilState::~D3D11StencilState()
    {
        if (m_depth_stencil_state)
            m_depth_stencil_state->Release();
    }

    void D3D11StencilState::initialize()
    {
        HRESULT result = m_device->CreateDepthStencilState(&m_depth_stencil_desc, &m_depth_stencil_state);
        if (FAILED(result))
            printf("Could not create stencil state\n");
	}

    void D3D11StencilState::setState()
    {
        m_device_context->OMSetDepthStencilState(m_depth_stencil_state, 1);
    }
}