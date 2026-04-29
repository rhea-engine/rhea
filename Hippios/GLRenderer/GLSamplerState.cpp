/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "GLSamplerState.h"

namespace hippios
{
	GLSamplerState::GLSamplerState(const SamplerDesc& desc)
	{
		glGenSamplers(1, &m_sampler);

		GLenum min_filter = (desc.filter == SamplerDesc::Filter::LINEAR) ? GL_LINEAR : GL_NEAREST;
		GLenum mag_filter = min_filter;

		GLenum wrap_s = (desc.address_u == SamplerDesc::AddressMode::WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
		GLenum wrap_t = (desc.address_v == SamplerDesc::AddressMode::WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
		GLenum wrap_r = (desc.address_w == SamplerDesc::AddressMode::WRAP) ? GL_REPEAT : GL_CLAMP_TO_EDGE;

		glSamplerParameteri(m_sampler, GL_TEXTURE_MIN_FILTER, min_filter);
		glSamplerParameteri(m_sampler, GL_TEXTURE_MAG_FILTER, mag_filter);
		glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_S, wrap_s);
		glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_T, wrap_t);
		glSamplerParameteri(m_sampler, GL_TEXTURE_WRAP_R, wrap_r);
		glSamplerParameterf(m_sampler, GL_TEXTURE_MIN_LOD, desc.min_lod);
		glSamplerParameterf(m_sampler, GL_TEXTURE_MAX_LOD, desc.max_lod);
	}

	GLSamplerState::~GLSamplerState()
	{
		if (m_sampler)
			glDeleteSamplers(1, &m_sampler);
	}
}