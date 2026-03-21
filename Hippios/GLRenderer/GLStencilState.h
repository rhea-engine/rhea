#ifndef GL_STENCILSTATE_H
#define GL_STENCILSTATE_H

#include <Hippios/StencilState.h>
#include "GLIncludes.h"

namespace Hippios
{
	class GLStencilState : public StencilState
	{
	public:
		GLStencilState();
		GLStencilState(const DepthStencilStateDesc& desc);

		void setState(int stencil_ref);

		const DepthStencilStateDesc& getDesc() const { return m_desc; }

	private:
		DepthStencilStateDesc m_desc;
	};

	GLenum toGLStencilOp(StencilOp op);
}
#endif // !GL_STENCILSTATE_H