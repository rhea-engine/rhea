#include "GLShader.h"
#include <fstream>
#include <sstream>
#include <Psio/Log.h>

namespace Hippios
{
	GLShader::GLShader() = default;

	GLShader::~GLShader()
	{
		if (m_shader)
			glDeleteShader(m_shader);
	}

	static const char* shaderTypeName(GLenum type)
	{
		switch (type) {
		case GL_VERTEX_SHADER:   return "Vertex";
		case GL_FRAGMENT_SHADER: return "Fragment";
		case GL_GEOMETRY_SHADER: return "Geometry";
		default:                 return "Unknown";
		}
	}

	bool GLShader::loadFromFile(const std::string& filename, const std::string& entryPoint, GLenum type)
	{
		std::ifstream file(filename);
		if (!file.is_open()) {
			LOG(LOG_ERROR) << "GLShader: Failed to open " << shaderTypeName(type) << " shader file: " << filename;
			return false;
		}

		std::stringstream ss;
		ss << file.rdbuf();
		std::string source = ss.str();

		if (!compile(source.c_str(), source.size(), type)) {
			LOG(LOG_ERROR) << "GLShader: " << shaderTypeName(type) << " shader compilation failed for file: " << filename;
			return false;
		}
		return true;
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
			GLint log_length = 0;
			glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &log_length);
			std::string info_log(log_length > 0 ? log_length : 512, '\0');
			glGetShaderInfoLog(m_shader, (GLsizei)info_log.size(), nullptr, &info_log[0]);
			LOG(LOG_ERROR) << "GLShader: " << shaderTypeName(type) << " compilation failed:\n" << info_log;
			glDeleteShader(m_shader);
			m_shader = 0;
			return false;
		}

		return true;
	}
}