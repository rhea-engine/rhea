#ifndef POSEIDON_SHADER_H
#define POSEIDON_SHADER_H

#include <string>

#include "Hippios/Renderer.h"
#include <Hippios/GPUProgram.h>
#include <Hippios/GraphicsBuffer.h>
#include <Hippios/PipelineState.h>
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

		enum class MatBufferLayoutElementType
		{
			VECTOR3 = 0,
			VECTOR4 = 1,
			INT = 2,
			FLOAT = 3
		};

		struct MatBufferLayoutElement
		{
			MatBufferLayoutElementType type;
			std::string name;
			size_t offset;
			size_t size;

			MatBufferLayoutElement(MatBufferLayoutElementType type, std::string name, size_t offset, size_t size) : type(type), name(name), offset(offset), size(size) {}
		};

		// get the layout for the material buffer
		std::vector<MatBufferLayoutElement>* getMatBufLayout() { return &m_matbuf_layout; }
		// add element for material buffer layoout
		void addMatBufLayoutElement(MatBufferLayoutElementType type, std::string name, size_t offset, size_t size) {
			m_matbuf_layout.push_back(MatBufferLayoutElement( type, name, offset, size ));
			m_matbuf_size = offset + size;
			m_matbuf_layout_map[name] = (int)m_matbuf_layout.size() - 1;
		}
		void setMatBufLayout(std::vector<MatBufferLayoutElement>& layout);
		size_t getMatBufSize() { return m_matbuf_size; }
		int getMatBufLayoutMap(std::string name) { if (m_matbuf_layout_map.find(name) != m_matbuf_layout_map.end()) return m_matbuf_layout_map[name]; else return -1; }

	protected:
		Core* m_grahpics_core;

		int m_vertex_type;

		Hippios::Renderer* m_renderer;
		Hippios::GPUProgram* m_vshader;
		Hippios::GPUProgram* m_pshader;

		bool m_alpha_blending;
		std::unique_ptr<Hippios::PipelineStateObject> m_pso;
		std::string m_stencil_test;
		int m_topology_type;
		bool m_cull_mode;

		std::vector<MatBufferLayoutElement> m_matbuf_layout;
		size_t m_matbuf_size;
		std::unordered_map<std::string, int> m_matbuf_layout_map;
		Hippios::GraphicsBuffer* m_material_buffer;
	};
}

#endif // !POSEIDON_SHADER_H
