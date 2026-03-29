#include "GLStencilState.h"
#include "GLBaseTypes.h"

namespace Hippios
{
	GLenum toGLStencilOp(StencilOp op)
	{
		switch (op) {
		case StencilOp::STENCIL_KEEP: return GL_KEEP;
		case StencilOp::STENCIL_ZERO: return GL_ZERO;
		case StencilOp::STENCIL_REPLACE: return GL_REPLACE;
		case StencilOp::STENCIL_INCR_SAT: return GL_INCR;
		case StencilOp::STENCIL_DECR_SAT: return GL_DECR;
		case StencilOp::STENCIL_INVERT: return GL_INVERT;
		case StencilOp::STENCIL_INCR: return GL_INCR_WRAP;
		case StencilOp::STENCIL_DECR: return GL_DECR_WRAP;
		default: return GL_KEEP;
		}
	}

	GLStencilState::GLStencilState(const DepthStencilStateDesc& desc) :
		m_desc(desc)
	{
	}

	void GLStencilState::setState(int stencil_ref)
	{
		// Depth
		if (m_desc.depth_enable) {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(gl_comparison_functions[m_desc.depth_func]);
			glDepthMask(m_desc.depth_write_mask == DepthWriteMask::DEPTH_WRITE_ALL ? GL_TRUE : GL_FALSE);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}

		// Stencil
		if (m_desc.stencil_enable) {
			glEnable(GL_STENCIL_TEST);
			glStencilMask(m_desc.stencil_write_mask);

			// Front face
			glStencilFuncSeparate(GL_FRONT,
				gl_comparison_functions[m_desc.front_face.stencil_func],
				stencil_ref, m_desc.stencil_read_mask);
			glStencilOpSeparate(GL_FRONT,
				toGLStencilOp(m_desc.front_face.stencil_fail_op),
				toGLStencilOp(m_desc.front_face.depth_fail_op),
				toGLStencilOp(m_desc.front_face.pass_op));

			// Back face
			glStencilFuncSeparate(GL_BACK,
				gl_comparison_functions[m_desc.back_face.stencil_func],
				stencil_ref, m_desc.stencil_read_mask);
			glStencilOpSeparate(GL_BACK,
				toGLStencilOp(m_desc.back_face.stencil_fail_op),
				toGLStencilOp(m_desc.back_face.depth_fail_op),
				toGLStencilOp(m_desc.back_face.pass_op));
		}
		else {
			glDisable(GL_STENCIL_TEST);
		}
	}
}