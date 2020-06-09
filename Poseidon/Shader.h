#ifndef POSEIDON_SHADER_H
#define POSEIDON_SHADER_H

#include <string>

#include "Hippios/Renderer.h"
#include <Hippios/GPUProgram.h>
#include <Hippios/GraphicsBuffer.h>

namespace Poseidon
{
	class Shader
	{
	public:
		Shader(Hippios::Renderer* renderer);
		~Shader();

		// set the vertex type
		void setVertexType(int type) { m_vertex_type = type; }

		// get the vertex type
		int getVertexType() { return m_vertex_type; }

		// load a shader from file returns true on success
		bool loadFromFile(std::string filename, char* vs_entry_point, char* ps_entry_point);

		// load a shader from a char buffer
		bool loadFromBuffer(unsigned char* buf, size_t buflen, char* vs_entry_point, char* ps_entry_point);

		// return the vertex shader
		Hippios::GPUProgram* getVShader() { return m_vshader; }

		// return the pixel shader
		Hippios::GPUProgram* getPShader() { return m_pshader; }

	protected:
		int m_vertex_type;

		Hippios::Renderer* m_renderer;
		Hippios::GPUProgram* m_vshader;
		Hippios::GPUProgram* m_pshader;
	};
}

#endif // !POSEIDON_SHADER_H
