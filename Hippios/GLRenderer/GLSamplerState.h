#ifndef GL_SAMPLERSTATE_H
#define GL_SAMPLERSTATE_H

#include <Hippios/SamplerState.h>
#include "GLIncludes.h"

namespace hippios
{
	class GLSamplerState : public SamplerState
	{
	public:
		GLSamplerState(const SamplerDesc& desc);
		~GLSamplerState();

		GLuint getSampler() const { return m_sampler; }

	private:
		GLuint m_sampler = 0;
	};
}

#endif // !GL_SAMPLERSTATE_H