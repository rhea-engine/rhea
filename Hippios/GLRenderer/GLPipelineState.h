#ifndef GL_PIPELINESTATE_H
#define GL_PIPELINESTATE_H

#include <Hippios/PipelineState.h>
#include "GLStencilState.h"
#include "GLBlendState.h"
#include <memory>
#include <vector>
#include "GLIncludes.h"

namespace hippios
{
	class GLShaderProgram;

	class GLPipelineStateObject : public PipelineStateObject
	{
	public:
		GLPipelineStateObject(const GLCapabilities& caps, const PipelineStateObjectDesc& desc);
		~GLPipelineStateObject();

		void SetState() override;

		GLuint getProgram() const { return m_program; }
		GLuint getVAO() const { return m_vao; }
		const InputLayoutDesc& getInputLayout() const { return m_inputLayout; }

		GLenum getTopology() const { return m_topology; }

	private:
		void createInputLayout(const InputLayoutDesc& layout);
		
		GLuint m_program = 0;
		GLuint m_vao = 0;
		GLenum m_topology;
		CullMode m_cull_mode;
		FillMode m_fill_mode;

		std::unique_ptr<GLStencilState> m_stencil_state;
		std::unique_ptr<GLBlendState> m_blend_state;
		GLShaderProgram* m_shader_program = nullptr;

		std::vector<InputLayoutElement> m_elements;
		std::vector<InputBindingDesc> m_bindings;
		InputLayoutDesc m_inputLayout{};

		GLCapabilities m_caps;
	};

	GLenum toGLTopology(PrimitiveTopology topo);
}
#endif // !GL_PIPELINESTATE_H