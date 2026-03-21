#include "GLShader.h"
#include <fstream>
#include <sstream>
#include <cstdio>

namespace Hippios
{
	GLShader::GLShader() = default;

	GLShader::~GLShader()
	{
		if (m_shader)
			glDeleteShader(m_shader);
	}

	bool GLShader::loadFromFile(const std::string& filename, const std::string& entryPoint, GLenum type)
	{
		std::ifstream file(filename);
		if (!file.is_open()) {
			printf("GLShader: Failed to open shader file: %s\n", filename.c_str());
			return false;
		}

		std::stringstream ss;
		ss << file.rdbuf();
		std::string source = ss.str();

		return compile(source.c_str(), source.size(), type);
	}

	bool GLShader::loadFromSource(const unsigned char* buf, size_t buflen, const std::string& entryPoint, GLenum type)
	{
		return compile(reinterpret_cast<const char*>(buf), buflen, type);
	}

	bool GLShader::compile(const char* source, size_t length, GLenum type)
	{
		m_shader_type = type;

		m_shader = glCreateShader(type);
		const GLint len = static_cast<GLint>(length);
		glShaderSource(m_shader, 1, &source, &len);
		glCompileShader(m_shader);

		GLint success;
		glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char info_log[512];
			glGetShaderInfoLog(m_shader, 512, nullptr, info_log);
			printf("GLShader: Compilation failed: %s\n", info_log);
			glDeleteShader(m_shader);
			m_shader = 0;
			return false;
		}

		return true;
	}
}