#ifndef HIPPIOS_D3D11_PIPELINESTATE_H
#define HIPPIOS_D3D11_PIPELINESTATE_H

#include "../PipelineState.h"
#include "../Renderer.h"
#include <D3D11StencilState.h>
#include <D3D11BlendState.h>
#include <memory>
#include <D3D11Renderer.h>

namespace Hippios
{
	class D3D11PipelineStateObject : public PipelineStateObject
	{
	public:
		D3D11PipelineStateObject(Renderer* renderer, PipelineStateObjectDesc* desc);
		~D3D11PipelineStateObject();

		void SetState() override;
	
	protected:
		D3D11StencilState* m_stencil_state;
		std::unique_ptr<D3D11BlendState> m_blend_state;
		D3D11Renderer* m_renderer;
		// legacy
		GPUProgram* m_vertex_shader;
		GPUProgram* m_pixel_shader;
		int m_topology;
		CullMode m_cull_mode;
		ID3D11RasterizerState* m_raster_state;
	};
}
#endif // !HIPPIOS_D3D11_PIPELINESTATE_H
