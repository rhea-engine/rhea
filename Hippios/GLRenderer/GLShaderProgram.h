#ifndef GL_SHADERPROGRAM_H
#define GL_SHADERPROGRAM_H

#include <Hippios/ShaderProgram.h>
#include "GLIncludes.h"
#include <memory>
#include <string>

namespace Hippios
{
	class GLShader;

	class GLShaderProgram : public ShaderProgram
	{
	public:
		GLShaderProgram();
		~GLShaderProgram() override;

		bool loadFromFiles(
			const std::string& vertexFile,
			const std::string& vertexEntry,
			const std::string& pixelFile,
			const std::string& pixelEntry);

		bool loadFromSource(
			const unsigned char* vertexSource,
			size_t vertexLength,
			const std::string& vertexEntry,
			const unsigned char* pixelSource,
			size_t pixelLength,
			const std::string& pixelEntry);

		GLuint getProgram() const { return m_program; }
		GLShader* getVertexShader() const { return m_vertex_shader.get(); }
		GLShader* getPixelShader() const { return m_pixel_shader.get(); }
		
		// Binds a texture sampler uniform to a specific texture slot (0-based).
		void bindSampler(const char* uniformName, int slot);

		void setSamplerBindings(const std::vector<SamplerBindingDesc>& bindings)
		{
			m_sampler_bindings = bindings;
		}

	private:
		bool linkProgram();

		GLuint m_program = 0;
		std::unique_ptr<GLShader> m_vertex_shader;
		std::unique_ptr<GLShader> m_pixel_shader;

		std::vector<SamplerBindingDesc> m_sampler_bindings;
	};
}

#endif // !GL_SHADERPROGRAM_H