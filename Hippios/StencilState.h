#ifndef HIPPIOS_STENCILSTATE_H
#define HIPPIOS_STENCILSTATE_H

#include <cstdint>
#include <string>
#include "BaseTypes.h"

namespace Hippios
{
	enum class StencilOp
	{
		STENCIL_KEEP,
		STENCIL_ZERO,
		STENCIL_REPLACE,
		STENCIL_INCR_SAT,
		STENCIL_DECR_SAT,
		STENCIL_INVERT,
		STENCIL_INCR,
		STENCIL_DECR
	};

	struct StencilOpDesc
	{
		StencilOp stencil_fail_op = StencilOp::STENCIL_KEEP;
		StencilOp depth_fail_op = StencilOp::STENCIL_KEEP;
		StencilOp pass_op = StencilOp::STENCIL_KEEP;
		ComparisonFunction stencil_func = ComparisonFunction::COMPARE_ALWAYS;
	};

	enum class DepthWriteMask
	{
		DEPTH_WRITE_ZERO = 0,
		DEPTH_WRITE_ALL = 1
	};


	struct DepthStencilStateDesc
	{
		// name of this sate
		std::string name;

		// wheter or not to enable depth
		bool depth_enable = false;

		// wheter or not to write to the depth buffer
		bool depth_write_enable = false;

		// define depth write mask
		DepthWriteMask depth_write_mask = DepthWriteMask::DEPTH_WRITE_ZERO;

		// define bits for stencil read mask
		uint8_t stencil_read_mask = 0x00;

		// define bits for stencil write mask
		uint8_t stencil_write_mask = 0x00;

		// wheter or not to enable stencil
		bool stencil_enable = false;

		// the depth comparison function
		ComparisonFunction depth_func = ComparisonFunction::COMPARE_LESS;

		StencilOpDesc front_face;
		StencilOpDesc back_face;
	};

	class StencilState
	{
	public:
		virtual ~StencilState() = default;
	};
}

#endif // !HIPPIOS_STENCILSTATE_H
