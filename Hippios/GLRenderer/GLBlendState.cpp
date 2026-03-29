#include "GLBlendState.h"

namespace Hippios
{
	GLBlendState::GLBlendState(const BlendStateDesc& desc) :
		m_desc(desc)
	{
	}

	GLenum GLBlendState::toGLBlendFactor(BlendFactor f)
	{
		switch (f) {
		case BlendFactor::BLEND_ONE: return GL_ONE;
		case BlendFactor::BLEND_ZERO: return GL_ZERO;
		case BlendFactor::BLEND_SRC_COLOR: return GL_SRC_COLOR;
		case BlendFactor::BLEND_INV_SRC_COLOR: return GL_ONE_MINUS_SRC_COLOR;
		case BlendFactor::BLEND_SRC_ALPHA: return GL_SRC_ALPHA;
		case BlendFactor::BLEND_INV_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::BLEND_DEST_ALPHA: return GL_DST_ALPHA;
		case BlendFactor::BLEND_INV_DEST_ALPHA: return GL_ONE_MINUS_DST_ALPHA;
		case BlendFactor::BLEND_DEST_COLOR: return GL_DST_COLOR;
		case BlendFactor::BLEND_INV_DEST_COLOR: return GL_ONE_MINUS_DST_COLOR;
		case BlendFactor::BLEND_SRC_ALPHA_SAT: return GL_SRC_ALPHA_SATURATE;
		default: return GL_ONE;
		}
	}

	GLenum GLBlendState::toGLBlendOp(BlendOp op)
	{
		switch (op) {
		case BlendOp::BLEND_ADD: return GL_FUNC_ADD;
		case BlendOp::BLEND_SUBTRACT: return GL_FUNC_SUBTRACT;
		case BlendOp::BLEND_REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
		case BlendOp::BLEND_MIN: return GL_MIN;
		case BlendOp::BLEND_MAX: return GL_MAX;
		default: return GL_FUNC_ADD;
		}
	}

	void GLBlendState::setState(float* blend_factor, unsigned int mask)
	{
		auto& rt = m_desc.renderTarget[0];
		if (rt.blend_enable) {
			glEnable(GL_BLEND);
			glBlendFuncSeparate(
				toGLBlendFactor(rt.src_blend),
				toGLBlendFactor(rt.dest_blend),
				toGLBlendFactor(rt.src_blend_alpha),
				toGLBlendFactor(rt.dest_blend_alpha));
			glBlendEquationSeparate(
				toGLBlendOp(rt.blend_op),
				toGLBlendOp(rt.blend_op_alpha));
		}
		else {
			glDisable(GL_BLEND);
		}
	}
}