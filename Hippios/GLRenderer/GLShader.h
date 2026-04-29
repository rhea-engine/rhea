/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef GL_SHADER_H
#define GL_SHADER_H

#include <string>
#include <vector>
#include "GLIncludes.h"

namespace hippios
{
	/// Internal helper: compiles a single GL shader stage (vertex or fragment).
	/// Not part of the public API — used by GLShaderProgram.
	class GLShader
	{
	public:
		GLShader();
		~GLShader();

		// prevent copy (owns GL resource)
		GLShader(const GLShader&) = delete;
		GLShader& operator=(const GLShader&) = delete;

		bool loadFromFile(const std::string& filename, const std::string& entryPoint, GLenum type);
		bool loadFromSource(const unsigned char* buf, size_t buflen, const std::string& entryPoint, GLenum type);

		GLuint getShader() const { return m_shader; }
		GLenum getShaderType() const { return m_shader_type; }

	private:
		bool compile(const char* source, size_t length, GLenum type);

		GLuint m_shader = 0;
		GLenum m_shader_type = GL_VERTEX_SHADER;
	};
}
#endif // !GL_SHADER_H