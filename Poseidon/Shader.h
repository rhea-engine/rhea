#ifndef POSEIDON_SHADER_H
#define POSEIDON_SHADER_H

#include <string>

#include "Hippios/Renderer.h"
#include <Hippios/GPUProgram.h>
#include <Hippios/GraphicsBuffer.h>
#include <Poseidon/Core.h>

namespace Poseidon
{
	class Shader
	{
	public:
		Shader(Core* core);
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

		// set alpha blending
		void setAlphaBlending(bool enable) { m_alpha_blending = enable; }

		// set stencil testing to stencil test with given name
		void setStencilTest(std::string test_name) { m_stencil_test = test_name; }

		// cull mode
		void setCullMode(bool enabled) { m_cull_mode = enabled; }

		// setup is complete, finalize
		void finalize();

		// set the current shader as active shader
		void activate();

	protected:
		Core* m_grahpics_core;

		int m_vertex_type;

		Hippios::Renderer* m_renderer;
		Hippios::GPUProgram* m_vshader;
		Hippios::GPUProgram* m_pshader;

		bool m_alpha_blending;
		Hippios::PipelineStateObject* m_pso;
		std::string m_stencil_test;
		int m_topology_type;
		bool m_cull_mode;
	};
}

#endif // !POSEIDON_SHADER_H
