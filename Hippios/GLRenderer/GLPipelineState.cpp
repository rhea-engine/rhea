#include "GLPipelineState.h"
#include "GLShaderProgram.h"
#include <cstdio>
#include <Psio/Log.h>
#include "GLIncludes.h"

namespace hippios
{
	GLPipelineStateObject::GLPipelineStateObject(const GLCapabilities& caps, const PipelineStateObjectDesc& desc) :
		m_caps(caps),
		m_shader_program(static_cast<GLShaderProgram*>(desc.shaderProgram)),
		m_cull_mode(desc.rasterizerState.cull_mode),
		m_fill_mode(desc.rasterizerState.fill_mode)
	{
		m_topology = toGLTopology(desc.primitiveTopology);

		// Use the already-linked program from GLShaderProgram
		if (m_shader_program)
			m_program = m_shader_program->getProgram();

		if (desc.inputLayout.elements && desc.inputLayout.num_elements > 0)
		{
			m_elements.assign(
				desc.inputLayout.elements,
				desc.inputLayout.elements + desc.inputLayout.num_elements);

			m_inputLayout.elements = m_elements.data();
			m_inputLayout.num_elements = static_cast<int>(m_elements.size());
		}

		if (desc.inputLayout.bindings && desc.inputLayout.num_bindings > 0)
		{
			m_bindings.assign(
				desc.inputLayout.bindings,
				desc.inputLayout.bindings + desc.inputLayout.num_bindings);

			m_inputLayout.bindings = m_bindings.data();
			m_inputLayout.num_bindings = static_cast<int>(m_bindings.size());
		}

		// Create VAO for input layout
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		if (desc.inputLayout.num_elements > 0)
			createInputLayout(m_inputLayout);

		// Stencil/depth state
		m_stencil_state = std::make_unique<GLStencilState>(desc.depthStencilState);

		// Blend state
		m_blend_state = std::make_unique<GLBlendState>(desc.blendState);
	}
	
	GLPipelineStateObject::~GLPipelineStateObject()
	{
		// Don't delete m_program here — it's owned by GLShaderProgram
		if (m_vao)
			glDeleteVertexArrays(1, &m_vao);
	}

	void GLPipelineStateObject::createInputLayout(const InputLayoutDesc& layout)
	{
		if (layout.num_elements <= 0 || layout.elements == nullptr)
			return;

		if (m_caps.hasDSA)
		{
			// Define attribute formats and connect each attribute to a VAO binding slot.
			for (int i = 0; i < layout.num_elements; ++i)
			{
				const auto& elem = layout.elements[i];

				GLuint attribIndex = static_cast<GLuint>(elem.location);
				GLuint bindingIndex = static_cast<GLuint>(elem.input_slot);
				GLuint relativeOffset = static_cast<GLuint>(elem.aligned_byte_offset);

				int components = getFormatComponentCount(elem.format);
				GLenum glType = getFormatType(elem.format);

				if (components <= 0)
				{
					printf("Unsupported input layout format in createInputLayout()\n");
					continue;
				}

				glEnableVertexArrayAttrib(m_vao, attribIndex);

				if (isIntegerFormat(elem.format))
				{
					// Integer shader input: e.g. layout(location=4) in uvec4 joints;
					glVertexArrayAttribIFormat(
						m_vao,
						attribIndex,
						components,
						glType,
						relativeOffset);
				}
				else
				{
					// Floating-point shader input: vec2/vec3/vec4 etc.
					glVertexArrayAttribFormat(
						m_vao,
						attribIndex,
						components,
						glType,
						isNormalizedFormat(elem.format) ? GL_TRUE : GL_FALSE,
						relativeOffset);
				}

				// Tell OpenGL which vertex buffer binding slot this attribute reads from.
				glVertexArrayAttribBinding(m_vao, attribIndex, bindingIndex);
			}

			// Define per-binding instancing behavior.
			for (int i = 0; i < layout.num_bindings; ++i)
			{
				const auto& binding = layout.bindings[i];
				glVertexArrayBindingDivisor(
					m_vao,
					static_cast<GLuint>(binding.inputSlot),
					static_cast<GLuint>(binding.instanceStepRate));
			}
		}
		else {
			glBindVertexArray(m_vao);

			for (int i = 0; i < layout.num_elements; ++i)
			{
				const auto& elem = layout.elements[i];
				glEnableVertexAttribArray(static_cast<GLuint>(elem.location));
			}
		}
	}

	void GLPipelineStateObject::SetState()
	{
		glUseProgram(m_program);
		glBindVertexArray(m_vao);

		// Rasterizer state
		if (m_fill_mode == FillMode::WIREFRAME)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
		switch (m_cull_mode) {
		case CullMode::FRONT:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			break;
		case CullMode::BACK:
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			break;
		case CullMode::NONE:
			glDisable(GL_CULL_FACE);
			break;
		}
		glDisable(GL_CULL_FACE);
		
		// Stencil/Depth
		m_stencil_state->setState(0);

		// Blend
		m_blend_state->setState(nullptr, 0xFFFFFFFF);		
	}
}