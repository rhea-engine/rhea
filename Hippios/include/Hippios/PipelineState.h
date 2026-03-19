#ifndef HIPPIOS_PSO_H
#define HIPPIOS_PSO_H

#include "StencilState.h"
#include "ShaderProgram.h"

namespace Hippios
{
	class PipelineStateObject
	{
	public:
		virtual ~PipelineStateObject() = default;

		virtual void SetState() = 0;
	};

	enum class FillMode {
		SOLID = 0,
		WIREFRAME = 1
	};
	enum class CullMode { 
		FRONT = 0, 
		BACK = 1, 
		NONE = 2 
	};
	struct RasterizerStateDesc { 
		FillMode fill_mode = FillMode::SOLID;
		CullMode cull_mode = CullMode::BACK;
	};
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
	
	enum class PrimitiveTopology { 
		TriangleList = 0, 
		TriangleStrip = 1, 
		LineList = 2, 
		LineStrip = 3, 
		PointList = 4 
	};

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
	
	enum class InputLayoutFormat {
		FORMAT_UNKNOWN = 0,
		R32G32B32A32_FLOAT,
		R32G32B32_FLOAT,
		R32G32_FLOAT,
		R32_FLOAT,
		R32G32B32A32_UINT,
		R32G32B32_UINT,
		R32G32_UINT,
		R32_UINT,
		R32G32B32A32_SINT,
		R32G32B32_SINT,
		R32G32_SINT,
		R32_SINT,
		R16G16B16A16_FLOAT,
		R16G16_FLOAT,
		R16_FLOAT,
		R16G16B16A16_UINT,
		R16G16_UINT,
		R16_UINT,
		R16G16B16A16_SINT,
		R16G16_SINT,
		R16_SINT,
		R8G8B8A8_UNORM,
		R8G8_UNORM,
		R8_UNORM,
		R8G8B8A8_UINT,
		R8G8_UINT,
		R8_UINT,
		R8G8B8A8_SINT,
		R8G8_SINT,
		R8_SINT

	};
	struct InputLayoutElement {
		const char* semantic_name = "ATTRIB";
		int semantic_index = 0;
		InputLayoutFormat format = InputLayoutFormat::FORMAT_UNKNOWN;
		int input_slot = 0;
		int aligned_byte_offset = 0;
		int instance_data_step_rate = 0;
	};
	struct InputLayoutDesc {
		InputLayoutElement* elements = nullptr;
		int num_elements = 0;
	};


	struct PipelineStateObjectDesc
	{
		DepthStencilStateDesc depthStencilState;
		RasterizerStateDesc rasterizerState;
		BlendStateDesc blendState;
		ShaderProgram* shaderProgram = nullptr;
		PrimitiveTopology vertexLayout = PrimitiveTopology::TriangleList;
		bool m_alpha_blending = false;
		InputLayoutDesc inputLayout;
	};
}
#endif // !HIPPIOS_PSO_H
