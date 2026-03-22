#ifndef POSEIDON_SHADER_H
#define POSEIDON_SHADER_H

#include <string>

#include "Hippios/Renderer.h"
#include <Hippios/ShaderProgram.h>
#include <Hippios/GraphicsBuffer.h>
#include <Hippios/PipelineState.h>
#include <Hippios/SamplerState.h>
#include <Poseidon/Core.h>

namespace Poseidon
{
	struct ShaderProgramSourceDesc
	{
		const unsigned char* vertexSource = nullptr;
		size_t vertexSourceLength = 0;

		const unsigned char* pixelSource = nullptr;
		size_t pixelSourceLength = 0;

		std::string vertexEntry; // used by D3D11
		std::string pixelEntry;  // used by D3D11
	};

	struct VertexInputAttribute
	{
		std::string semantic;
		uint32_t semanticIndex = 0;
		uint32_t location = 0;
		Hippios::InputLayoutFormat format = Hippios::InputLayoutFormat::FORMAT_UNKNOWN;
		uint32_t inputSlot = 0;
		uint32_t alignedByteOffset = 0;
		bool perInstance = false;
		uint32_t instanceStepRate = 0;
	};

	class Shader
	{
	public:
		Shader(Core* core);
		~Shader();

		// set the vertex input layout for this shader, with a single input slot and given stride
		void setInputLayout(const std::vector<VertexInputAttribute>& elements, uint32_t stride);
		// set the vertex input layout for this shader, with multiple input slots and given bindings
		void setInputLayout(const std::vector<VertexInputAttribute>& elements,
			const std::vector<Hippios::InputBindingDesc>& bindings);

		// load a shader from file returns true on success
		bool loadFromFile(std::string filename, char* vs_entry_point, char* ps_entry_point);

		// load a shader from a char buffer
		bool loadFromBuffer(unsigned char* buf, size_t buflen, char* vs_entry_point, char* ps_entry_point);

		// load a shader from description
		bool loadFromSourceDesc(const ShaderProgramSourceDesc& desc);

		// return the shader program
		Hippios::ShaderProgram* getShaderProgram() { return m_shader; }

		// set alpha blending
		void setAlphaBlending(bool enable) { m_alpha_blending = enable; }

		// set stencil testing to stencil test with given name
		void setStencilTest(std::string test_name) { m_stencil_test = test_name; }

		// set depth testing and writing
		void setDepthTest(bool enabled) { m_depth_test = enabled; }
		void setDepthWrite(bool enabled) { m_depth_write = enabled; }

		// cull mode
		void setCullMode(bool enabled) { m_cull_mode = enabled; }

		// sampler reference
		void setSamplerState(Hippios::SamplerState* sampler_state) { m_sampler_state = sampler_state; }

		// setup is complete, finalize
		void finalize();

		// set the current shader as active shader
		void activate();

		enum class MatBufferLayoutElementType
		{
			NONE = 0,
			FLOAT = 1,
			VECTOR2 = 2,
			VECTOR3 = 3,
			VECTOR4 = 4,
			INT = 5,
		};

		struct MatBufferLayoutElement
		{
			MatBufferLayoutElementType type;
			std::string name;
			size_t offset;
			size_t size;

			MatBufferLayoutElement(MatBufferLayoutElementType type, std::string name, size_t offset, size_t size) : type(type), name(name), offset(offset), size(size) {}
		};

		// get the layout for the PS material buffer
		std::vector<MatBufferLayoutElement>* getMatBufLayout() { return &m_matbuf_layout; }
		// add element for material buffer layoout
		void addMatBufLayoutElement(MatBufferLayoutElementType type, std::string name, size_t offset, size_t size) {
			m_matbuf_layout.push_back(MatBufferLayoutElement( type, name, offset, size ));
			m_matbuf_size = offset + size;
			m_matbuf_layout_map[name] = (int)m_matbuf_layout.size() - 1;
		}
		void setMatBufLayout(std::vector<MatBufferLayoutElement>& layout);
		size_t getMatBufSize() const { return m_matbuf_size; }
		int getMatBufLayoutMap(std::string name) { if (m_matbuf_layout_map.find(name) != m_matbuf_layout_map.end()) return m_matbuf_layout_map[name]; else return -1; }

		// get the layout for the VS material buffer
		std::vector<MatBufferLayoutElement>* getMatBufLayoutVS() { return &m_matbuf_layout_vs; }
		// add element for material buffer layoout
		void addMatBufLayoutElementVS(MatBufferLayoutElementType type, std::string name, size_t offset, size_t size) {
			m_matbuf_layout_vs.push_back(MatBufferLayoutElement(type, name, offset, size));
			m_matbuf_size_vs = offset + size;
			m_matbuf_layout_map_vs[name] = (int)m_matbuf_layout.size() - 1;
		}
		void setMatBufLayoutVS(std::vector<MatBufferLayoutElement>& layout);
		size_t getMatBufSizeVS() const { return m_matbuf_size_vs; }
		int getMatBufLayoutMapVS(std::string name) { if (m_matbuf_layout_map_vs.find(name) != m_matbuf_layout_map_vs.end()) return m_matbuf_layout_map_vs[name]; else return -1; }

	protected:
		Core* m_grahpics_core;

		std::vector<VertexInputAttribute> m_input_layout;
		std::vector<Hippios::InputBindingDesc> m_input_bindings;

		Hippios::Renderer* m_renderer;
		Hippios::ShaderProgram* m_shader;		

		bool m_alpha_blending;
		std::unique_ptr<Hippios::PipelineStateObject> m_pso;
		std::string m_stencil_test;
		int m_topology_type;
		bool m_cull_mode;
		Hippios::SamplerState* m_sampler_state;
		bool m_depth_test = true;
		bool m_depth_write = true;

		// pixel shader material buffer
		std::vector<MatBufferLayoutElement> m_matbuf_layout;
		size_t m_matbuf_size;
		std::unordered_map<std::string, int> m_matbuf_layout_map;
		Hippios::GraphicsBuffer* m_material_buffer;

		// vertex shader material buffer
		std::vector<MatBufferLayoutElement> m_matbuf_layout_vs;
		size_t m_matbuf_size_vs;
		std::unordered_map<std::string, int> m_matbuf_layout_map_vs;
		Hippios::GraphicsBuffer* m_material_buffer_vs;

		bool m_activate_error_logged = false;
	};
}

#endif // !POSEIDON_SHADER_H
