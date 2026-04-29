/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef GL_BLENDSTATE_H
#define GL_BLENDSTATE_H

#include <Hippios/BlendState.h>
#include <Hippios/PipelineState.h>
#include "GLIncludes.h"

namespace hippios
{
	class GLBlendState : public BlendState
	{
	public:
		GLBlendState(const BlendStateDesc& desc);

		void setState(float* blend_factor, unsigned int mask);

	private:
		BlendStateDesc m_desc;

		static GLenum toGLBlendFactor(BlendFactor f);
		static GLenum toGLBlendOp(BlendOp op);
	};
}
#endif // !GL_BLENDSTATE_H