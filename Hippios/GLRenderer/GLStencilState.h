/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef GL_STENCILSTATE_H
#define GL_STENCILSTATE_H

#include <Hippios/StencilState.h>
#include "GLIncludes.h"

namespace hippios
{
	class GLStencilState : public StencilState
	{
	public:
		GLStencilState(const DepthStencilStateDesc& desc);

		void setState(int stencil_ref);

		const DepthStencilStateDesc& getDesc() const { return m_desc; }

	private:
		DepthStencilStateDesc m_desc;
	};

	GLenum toGLStencilOp(StencilOp op);
}
#endif // !GL_STENCILSTATE_H