#ifndef HIPPIOS_D3D11SAMPLER_H
#define HIPPIOS_D3D11SAMPLER_H

#include <Hippios/SamplerState.h>
#include <d3d11.h>

namespace hippios
{
	class D3D11SamplerState : public SamplerState
	{
	public:
		ID3D11SamplerState* state = nullptr;
	};
}

#endif // !HIPPIOS_D3D11SAMPLER_H
