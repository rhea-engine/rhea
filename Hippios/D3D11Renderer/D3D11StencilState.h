#ifndef HIPPIOS_D3D11_STENCILSTATE_H
#define HIPPIOS_D3D11_STENCILSTATE_H

#include <Hippios/StencilState.h>
#include <d3d11.h>

namespace Hippios
{
	class D3D11StencilState : public StencilState
	{
	public:
		D3D11StencilState(ID3D11Device* device, ID3D11DeviceContext* device_context, const DepthStencilStateDesc& desc);
		~D3D11StencilState();

		void initialize();

		// activate the state
		void setState(unsigned int ref);

		DepthStencilStateDesc getDesc() const { return m_desc; }

	protected:
		DepthStencilStateDesc m_desc;
		D3D11_DEPTH_STENCIL_DESC m_depth_stencil_desc;
		ID3D11DepthStencilState* m_depth_stencil_state;
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_device_context;
	};
}

#endif // !HIPPIOS_D3D11_STENCILSTATE_H
