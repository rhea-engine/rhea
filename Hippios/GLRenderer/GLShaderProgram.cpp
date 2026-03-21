#include "GLShaderProgram.h"
#include "GLShader.h"
#include <cstdio>

namespace Hippios
{
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
			printf("GLShaderProgram: Failed to compile vertex shader: %s\n", vertexFile.c_str());
			return false;
		}

		m_pixel_shader = std::make_unique<GLShader>();
		if (!m_pixel_shader->loadFromFile(pixelFile, pixelEntry, GL_FRAGMENT_SHADER)) {
			printf("GLShaderProgram: Failed to compile fragment shader: %s\n", pixelFile.c_str());
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
			printf("GLShaderProgram: Failed to compile vertex shader from source\n");
			return false;
		}

		m_pixel_shader = std::make_unique<GLShader>();
		if (!m_pixel_shader->loadFromSource(pixelSource, pixelLength, pixelEntry, GL_FRAGMENT_SHADER)) {
			printf("GLShaderProgram: Failed to compile fragment shader from source\n");
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
			char info_log[512];
			glGetProgramInfoLog(m_program, 512, nullptr, info_log);
			printf("GLShaderProgram: Linking failed: %s\n", info_log);
			glDeleteProgram(m_program);
			m_program = 0;
			return false;
		}

		return true;
	}
}