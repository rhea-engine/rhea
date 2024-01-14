#ifndef HIPPIOS_D3D11_BLENDSTATE_H
#define HIPPIOS_D3D11_BLENDSTATE_H

#include <d3d11.h>
#include <Hippios/PipelineState.h>
#include <Hippios/BlendState.h>

namespace Hippios
{
	class D3D11BlendState : public BlendState
	{
	public:
		D3D11BlendState(ID3D11Device* device, BlendStateDesc& desc);
		~D3D11BlendState();

		ID3D11BlendState* getBlendState() { return m_state; }

	protected:
		ID3D11BlendState* m_state;

		D3D11_BLEND blend(BlendFactor factor);
		D3D11_BLEND_OP blendOp(BlendOp op);
	};
}
#endif // !HIPPIOS_D3D11_BLENDSTATE_H
