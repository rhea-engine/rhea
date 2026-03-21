#include "GLPipelineState.h"
#include "GLShaderProgram.h"
#include <cstdio>

namespace Hippios
{
	GLenum toGLTopology(PrimitiveTopology topo)
	{
		switch (topo) {
		case PrimitiveTopology::TriangleList: return GL_TRIANGLES;
		case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
		case PrimitiveTopology::LineList: return GL_LINES;
		case PrimitiveTopology::LineStrip: return GL_LINE_STRIP;
		case PrimitiveTopology::PointList: return GL_POINTS;
		default: return GL_TRIANGLES;
		}
	}

	static int getFormatComponentCount(InputLayoutFormat fmt)
	{
		switch (fmt)
		{
		case InputLayoutFormat::R32_FLOAT:
		case InputLayoutFormat::R32_UINT:
		case InputLayoutFormat::R32_SINT:
		case InputLayoutFormat::R16_FLOAT:
		case InputLayoutFormat::R16_UINT:
		case InputLayoutFormat::R16_SINT:
		case InputLayoutFormat::R8_UNORM:
		case InputLayoutFormat::R8_UINT:
		case InputLayoutFormat::R8_SINT:
			return 1;

		case InputLayoutFormat::R32G32_FLOAT:
		case InputLayoutFormat::R32G32_UINT:
		case InputLayoutFormat::R32G32_SINT:
		case InputLayoutFormat::R16G16_FLOAT:
		case InputLayoutFormat::R16G16_UINT:
		case InputLayoutFormat::R16G16_SINT:
		case InputLayoutFormat::R8G8_UNORM:
		case InputLayoutFormat::R8G8_UINT:
		case InputLayoutFormat::R8G8_SINT:
			return 2;

		case InputLayoutFormat::R32G32B32_FLOAT:
		case InputLayoutFormat::R32G32B32_UINT:
		case InputLayoutFormat::R32G32B32_SINT:
			return 3;

		case InputLayoutFormat::R32G32B32A32_FLOAT:
		case InputLayoutFormat::R32G32B32A32_UINT:
		case InputLayoutFormat::R32G32B32A32_SINT:
		case InputLayoutFormat::R16G16B16A16_FLOAT:
		case InputLayoutFormat::R16G16B16A16_UINT:
		case InputLayoutFormat::R16G16B16A16_SINT:
		case InputLayoutFormat::R8G8B8A8_UNORM:
		case InputLayoutFormat::R8G8B8A8_UINT:
		case InputLayoutFormat::R8G8B8A8_SINT:
			return 4;

		default:
			return 0;
		}
	}

	static GLenum getFormatType(InputLayoutFormat fmt)
	{
		switch (fmt)
		{
		case InputLayoutFormat::R32_FLOAT:
		case InputLayoutFormat::R32G32_FLOAT:
		case InputLayoutFormat::R32G32B32_FLOAT:
		case InputLayoutFormat::R32G32B32A32_FLOAT:
			return GL_FLOAT;

		case InputLayoutFormat::R32_UINT:
		case InputLayoutFormat::R32G32_UINT:
		case InputLayoutFormat::R32G32B32_UINT:
		case InputLayoutFormat::R32G32B32A32_UINT:
			return GL_UNSIGNED_INT;

		case InputLayoutFormat::R32_SINT:
		case InputLayoutFormat::R32G32_SINT:
		case InputLayoutFormat::R32G32B32_SINT:
		case InputLayoutFormat::R32G32B32A32_SINT:
			return GL_INT;

		case InputLayoutFormat::R16_FLOAT:
		case InputLayoutFormat::R16G16_FLOAT:
		case InputLayoutFormat::R16G16B16A16_FLOAT:
			return GL_HALF_FLOAT;

		case InputLayoutFormat::R16_UINT:
		case InputLayoutFormat::R16G16_UINT:
		case InputLayoutFormat::R16G16B16A16_UINT:
			return GL_UNSIGNED_SHORT;

		case InputLayoutFormat::R16_SINT:
		case InputLayoutFormat::R16G16_SINT:
		case InputLayoutFormat::R16G16B16A16_SINT:
			return GL_SHORT;

		case InputLayoutFormat::R8_UNORM:
		case InputLayoutFormat::R8G8_UNORM:
		case InputLayoutFormat::R8G8B8A8_UNORM:
		case InputLayoutFormat::R8_UINT:
		case InputLayoutFormat::R8G8_UINT:
		case InputLayoutFormat::R8G8B8A8_UINT:
			return GL_UNSIGNED_BYTE;

		case InputLayoutFormat::R8_SINT:
		case InputLayoutFormat::R8G8_SINT:
		case InputLayoutFormat::R8G8B8A8_SINT:
			return GL_BYTE;

		default:
			return GL_INVALID_ENUM;
		}
	}

	static bool isNormalizedFormat(InputLayoutFormat fmt)
	{
		switch (fmt)
		{
		case InputLayoutFormat::R8_UNORM:
		case InputLayoutFormat::R8G8_UNORM:
		case InputLayoutFormat::R8G8B8A8_UNORM:
			return true;
		default:
			return false;
		}
	}

	GLPipelineStateObject::GLPipelineStateObject(const PipelineStateObjectDesc& desc) :
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

		// Stencil/Depth
		m_stencil_state->setState(0);

		// Blend
		m_blend_state->setState(nullptr, 0xFFFFFFFF);
	}
}