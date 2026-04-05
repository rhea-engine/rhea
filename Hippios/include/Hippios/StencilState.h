#ifndef HIPPIOS_STENCILSTATE_H
#define HIPPIOS_STENCILSTATE_H

#include <cstdint>
#include <string>
#include "BaseTypes.h"

namespace hippios
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

		bool operator==(const StencilOpDesc& o) const
		{
			return stencil_fail_op == o.stencil_fail_op
				&& depth_fail_op == o.depth_fail_op
				&& pass_op == o.pass_op
				&& stencil_func == o.stencil_func;
		}
		bool operator!=(const StencilOpDesc& o) const { return !(*this == o); }
	};

	enum class DepthWriteMask
	{
		DEPTH_WRITE_ZERO = 0,
		DEPTH_WRITE_ALL = 1
	};


	struct DepthStencilStateDesc
	{
		// name of this state
		std::string name;

		// whether or not to enable depth
		bool depth_enable = true;

		// whether or not to write to the depth buffer
		bool depth_write_enable = true;

		// define depth write mask
		DepthWriteMask depth_write_mask = DepthWriteMask::DEPTH_WRITE_ALL;

		// define bits for stencil read mask
		uint8_t stencil_read_mask = 0xFF;

		// define bits for stencil write mask
		uint8_t stencil_write_mask = 0xFF;

		// whether or not to enable stencil
		bool stencil_enable = false;

		// the depth comparison function
		ComparisonFunction depth_func = ComparisonFunction::COMPARE_LESS;

		StencilOpDesc front_face;
		StencilOpDesc back_face;

		/// Compare all fields that affect the GPU state (ignores name).
		bool matches(const DepthStencilStateDesc& o) const
		{
			return depth_enable == o.depth_enable
				&& depth_write_enable == o.depth_write_enable
				&& depth_write_mask == o.depth_write_mask
				&& stencil_read_mask == o.stencil_read_mask
				&& stencil_write_mask == o.stencil_write_mask
				&& stencil_enable == o.stencil_enable
				&& depth_func == o.depth_func
				&& front_face == o.front_face
				&& back_face == o.back_face;
		}
	};

	class StencilState
	{
	public:
		virtual ~StencilState() = default;
	};
}

#endif // !HIPPIOS_STENCILSTATE_H
