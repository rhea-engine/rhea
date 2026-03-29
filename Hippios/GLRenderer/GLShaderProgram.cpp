#include "GLShaderProgram.h"
#include "GLShader.h"
#include <Psio/Log.h>

namespace Hippios
{
	/*
	CPU matrices are row - major / row - vector style
	GLSL therefore uses position * VP
	translation is read from[0][3], [1][3]
	*/

	static void bindUniformBlock(GLuint program, const char* blockName, GLuint binding)
	{
		GLuint blockIndex = glGetUniformBlockIndex(program, blockName);
		if (blockIndex == GL_INVALID_INDEX) {
			// Block not present in this shader; that's okay.
			return;
		}

		glUniformBlockBinding(program, blockIndex, binding);
	}

	static void bindSampler(GLuint program, const char* uniformName, GLint textureUnit)
	{
		GLint location = glGetUniformLocation(program, uniformName);
		if (location == -1) {
			return; // uniform not used in this shader
		}
		
		glUseProgram(program);
		glUniform1i(location, textureUnit);
	}

	void GLShaderProgram::bindSampler(const char* uniformName, int slot)
	{
		GLint location = glGetUniformLocation(m_program, uniformName);
		if (location == -1)
			return;

		glUseProgram(m_program);
		glUniform1i(location, slot);
	}

	GLShaderProgram::GLShaderProgram() = default;

	GLShaderProgram::~GLShaderProgram()
	{
		if (m_program)
			glDeleteProgram(m_program);
	}

	bool GLShaderProgram::loadFromFiles(
		const std::string& vertexFile,
		const std::string& vertexEntry,
		const std::string& pixelFile,
		const std::string& pixelEntry)
	{
		m_vertex_shader = std::make_unique<GLShader>();
		if (!m_vertex_shader->loadFromFile(vertexFile, vertexEntry, GL_VERTEX_SHADER)) {
			return false;
		}

		m_pixel_shader = std::make_unique<GLShader>();
		if (!m_pixel_shader->loadFromFile(pixelFile, pixelEntry, GL_FRAGMENT_SHADER)) {
			return false;
		}

		return linkProgram();
	}

	bool GLShaderProgram::loadFromSource(
		const unsigned char* vertexSource,
		size_t vertexLength,
		const std::string& vertexEntry,
		const unsigned char* pixelSource,
		size_t pixelLength,
		const std::string& pixelEntry)
	{
		m_vertex_shader = std::make_unique<GLShader>();
		if (!m_vertex_shader->loadFromSource(vertexSource, vertexLength, vertexEntry, GL_VERTEX_SHADER)) {
			LOG(LOG_ERROR) << "GLShaderProgram: Vertex shader compilation failed";
			return false;
		}

		m_pixel_shader = std::make_unique<GLShader>();
		if (!m_pixel_shader->loadFromSource(pixelSource, pixelLength, pixelEntry, GL_FRAGMENT_SHADER)) {
			LOG(LOG_ERROR) << "GLShaderProgram: Fragment shader compilation failed";
			return false;
		}

		return linkProgram();
	}

	bool GLShaderProgram::linkProgram()
	{
		m_program = glCreateProgram();

		if (m_vertex_shader)
			glAttachShader(m_program, m_vertex_shader->getShader());
		if (m_pixel_shader)
			glAttachShader(m_program, m_pixel_shader->getShader());

		glLinkProgram(m_program);

		GLint success;
		glGetProgramiv(m_program, GL_LINK_STATUS, &success);
		if (!success) {
			GLint log_length = 0;
			glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &log_length);
			std::string info_log(log_length > 0 ? log_length : 512, '\0');
			glGetProgramInfoLog(m_program, (GLsizei)info_log.size(), nullptr, &info_log[0]);
			LOG(LOG_ERROR) << "GLShaderProgram: Linking failed:\n" << info_log;
			glDeleteProgram(m_program);
			m_program = 0;
			return false;
		}

		bindUniformBlock(m_program, "cbPerFrame", 0);
		bindUniformBlock(m_program, "cbPerObject", 1);
		bindUniformBlock(m_program, "cbLight", 2);
		bindUniformBlock(m_program, "cbMaterial", 3);
		bindUniformBlock(m_program, "cbBone", 4);
		bindUniformBlock(m_program, "cbBackground", 5);

		for (const auto& b : m_sampler_bindings) {
			bindSampler(b.uniformName.c_str(), static_cast<int>(b.slot));
		}

		return true;
	}
}