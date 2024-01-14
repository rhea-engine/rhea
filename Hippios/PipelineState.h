#ifndef HIPPIOS_PSO_H
#define HIPPIOS_PSO_H

#include "StencilState.h"
#include "GPUProgram.h"

namespace Hippios
{
	class PipelineStateObject
	{
	public:
		virtual ~PipelineStateObject() = default;

		virtual void SetState() = 0;
	};

	struct RasterizerStateDesc { int i = 0; };
	
	enum class CullMode { FRONT = 0, BACK = 1, NONE = 2 };
	enum class BlendOp {
		BLEND_ADD,
		BLEND_SUBTRACT,
		BLEND_REVERSE_SUBTRACT,
		BLEND_MIN,
		BLEND_MAX
	};

	enum class BlendFactor { 
		BLEND_ONE, 
		BLEND_ZERO, 
		BLEND_SRC_COLOR, 
		BLEND_INV_SRC_COLOR,
		BLEND_SRC_ALPHA,
		BLEND_INV_SRC_ALPHA,
		BLEND_DEST_ALPHA,
		BLEND_INV_DEST_ALPHA,
		BLEND_DEST_COLOR,
		BLEND_INV_DEST_COLOR,
		BLEND_SRC_ALPHA_SAT,
		BLEND_BLEND_FACTOR,
		BLEND_INV_BLEND_FACTOR,
		BLEND_SRC1_COLOR,
		BLEND_INV_SRC1_COLOR,
		BLEND_SRC1_ALPHA,
		BLEND_INV_SRC1_ALPHA
	};
	enum class FillMode { Solid = 0, Wireframe = 1 };
	enum class PrimitiveTopology { TriangleList = 0, TriangleStrip = 1, LineList = 2, LineStrip = 3, PointList = 4 };

	struct RenderTargetBlendDesc { 
		bool blend_enable = false;
		BlendFactor src_blend = BlendFactor::BLEND_ONE;
		BlendFactor dest_blend = BlendFactor::BLEND_ZERO;
		BlendOp blend_op = BlendOp::BLEND_ADD;
		BlendFactor src_blend_alpha = BlendFactor::BLEND_ONE;
		BlendFactor dest_blend_alpha = BlendFactor::BLEND_ZERO;
		BlendOp blend_op_alpha = BlendOp::BLEND_ADD;
		int render_target_write_mask = 0;
	};

	struct BlendStateDesc { 
		bool alphaToCoverageEnable = false;
		bool independentBlendEnable = false;
		RenderTargetBlendDesc renderTarget[8];
	};
	
	struct ShaderProgramDesc { 
		GPUProgram* vertex_shader = nullptr;
		GPUProgram* pixel_shader = nullptr;
	};
	struct VertexLayoutDesc { int topology = 4; };

	struct PipelineStateObjectDesc
	{
		DepthStencilStateDesc depthStencilState;
		RasterizerStateDesc rasterizerState;
		BlendStateDesc blendState;
		ShaderProgramDesc shaderProgram;
		VertexLayoutDesc vertexLayout;
		bool m_alpha_blending = false;
		CullMode cull_mode = CullMode::FRONT;
	};
}
#endif // !HIPPIOS_PSO_H
