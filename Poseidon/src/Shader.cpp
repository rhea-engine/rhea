#include "Poseidon/Shader.h"
#include <cassert>
#include <Psio/Log.h>

namespace Poseidon
{
	Shader::Shader(Core* core) :
		m_grahpics_core(core),
		m_shader(nullptr),		
		m_alpha_blending(false),
		m_pso(nullptr),
		m_depth_stencil_state("default"),
		m_topology_type(0),
		m_cull_mode(true),
		m_matbuf_size(0),
		m_matbuf_size_vs(0),
		m_material_buffer(nullptr),
		m_matbuf_layout(),
		m_material_buffer_vs(nullptr),
		m_renderer(nullptr),
		m_sampler_state(nullptr)
	{
		m_renderer = m_grahpics_core->get_renderer();
	}

	Shader::~Shader()
	{
		if (m_shader)
			delete m_shader;		
	}

	void Shader::setInputLayout(const std::vector<VertexInputAttribute>& elements, uint32_t stride)
	{
		m_input_layout = elements;
		m_input_bindings.clear();
		m_input_bindings.push_back({ 0, stride, 0 });
	}

	void Shader::setInputLayout(const std::vector<VertexInputAttribute>& elements,
		const std::vector<Hippios::InputBindingDesc>& bindings)
	{
		m_input_layout = elements;
		m_input_bindings = bindings;
	}

	bool Shader::loadFromFile(std::string filename, char* vs_entry_point, char* ps_entry_point)
	{
		Hippios::ShaderProgramDesc desc;
		desc.vertexFile = filename;
		desc.vertexEntry = vs_entry_point ? vs_entry_point : "VSMain";
		desc.pixelFile = filename;
		desc.pixelEntry = ps_entry_point ? ps_entry_point : "PSMain";

		m_shader = m_renderer->createShader(desc);

		return (m_shader != nullptr);
	}

	bool Shader::loadFromBuffer(unsigned char* buf, size_t buflen, char* vs_entry_point, char* ps_entry_point)
	{
		Hippios::ShaderProgramSourceDesc desc;
		desc.vertexSource = buf;
		desc.vertexSourceLength = (int)buflen;
		desc.vertexEntry = vs_entry_point ? vs_entry_point : "VSMain";

		desc.pixelSource = buf;
		desc.pixelSourceLength = (int)buflen;
		desc.pixelEntry = ps_entry_point ? ps_entry_point : "PSMain";

		m_shader = m_renderer->createShader(desc);

		return (m_shader != nullptr);
	}

	bool Shader::loadFromSourceDesc(const ShaderProgramSourceDesc& desc)
	{
		Hippios::ShaderProgramSourceDesc source_desc;
		source_desc.vertexSource = desc.vertexSource;
		source_desc.vertexSourceLength = desc.vertexSourceLength;
		source_desc.vertexEntry = desc.vertexEntry;
		source_desc.pixelSource = desc.pixelSource;
		source_desc.pixelSourceLength = desc.pixelSourceLength;
		source_desc.pixelEntry = desc.pixelEntry;
		// set samplerBindings by iterating over desc.samplerBindings and converting them to Hippios::ShaderSamplerBinding
		source_desc.samplerBindings.reserve(desc.sampler_bindings.size());
		for (const auto& sb : desc.sampler_bindings) {
			Hippios::SamplerBindingDesc binding;
			binding.uniformName = sb.uniformName;
			binding.slot = sb.slot;
			source_desc.samplerBindings.push_back(binding);
		}		
		m_shader = m_renderer->createShader(source_desc);
		return (m_shader != nullptr);
	}

	void Shader::finalize()
	{
		if (!m_pso) {
			//  no compiled shader program, skipping PSO creation
			if (!m_shader) {
				LOG(LOG_ERROR) << "Shader::finalize: no compiled shader program, skipping PSO creation";
				return;
			}

			Hippios::PipelineStateObjectDesc pso_desc;

			// stencil state
			// start from default preset
			Hippios::DepthStencilStateDesc dsDesc;
			if (!m_grahpics_core->get_renderer()->getStencilStateDesc("default", dsDesc)) {
				dsDesc = Hippios::DepthStencilStateDesc{};
			}

			// apply named preset if present
			if (!m_depth_stencil_state.empty()) {
				Hippios::DepthStencilStateDesc presetDesc;
				if (m_grahpics_core->get_renderer()->getStencilStateDesc(m_depth_stencil_state, presetDesc)) {
					dsDesc = presetDesc;
				}
			}
			pso_desc.depthStencilState = dsDesc;

			if (m_has_depth_test_override) {
				// override depth test enable and write mask based on shader settings, but keep other stencil state settings from the preset
				pso_desc.depthStencilState.depth_enable = m_depth_test;
			}
			if (m_has_depth_write_override) {
				pso_desc.depthStencilState.depth_write_mask =
					m_depth_write ? Hippios::DepthWriteMask::DEPTH_WRITE_ALL
					: Hippios::DepthWriteMask::DEPTH_WRITE_ZERO;
			}

			// defaults
			pso_desc.blendState.renderTarget[0].src_blend = Hippios::BlendFactor::BLEND_SRC_ALPHA;//Hippios::BlendFactor::BLEND_ONE;
			pso_desc.blendState.renderTarget[0].dest_blend = Hippios::BlendFactor::BLEND_INV_SRC_ALPHA;
			pso_desc.blendState.renderTarget[0].blend_op = Hippios::BlendOp::BLEND_ADD;
			pso_desc.blendState.renderTarget[0].src_blend_alpha = Hippios::BlendFactor::BLEND_ONE;
			pso_desc.blendState.renderTarget[0].dest_blend_alpha = Hippios::BlendFactor::BLEND_ZERO;
			pso_desc.blendState.renderTarget[0].blend_op_alpha = Hippios::BlendOp::BLEND_ADD;
			pso_desc.blendState.renderTarget[0].render_target_write_mask = 0x0f;
			if (m_alpha_blending) {
				pso_desc.blendState.renderTarget[0].blend_enable = true;
			}

			// cull mode in raster state
			if (!m_cull_mode)
				pso_desc.rasterizerState.cull_mode = Hippios::CullMode::NONE;

			// shader programs
			pso_desc.shaderProgram = getShaderProgram();			

			std::vector<Hippios::InputLayoutElement> native_elements;
			native_elements.reserve(m_input_layout.size());

			for (const auto& e : m_input_layout) {
				Hippios::InputLayoutElement native{};
				native.semantic_name = e.semantic.c_str();
				native.semantic_index = e.semanticIndex;
				native.location = e.location;
				native.format = e.format;
				native.input_slot = e.inputSlot;
				native.aligned_byte_offset = e.alignedByteOffset;
				native_elements.push_back(native);
			}

			pso_desc.inputLayout.elements = native_elements.data();
			pso_desc.inputLayout.num_elements = (int)native_elements.size();
			pso_desc.inputLayout.bindings = m_input_bindings.data();
			pso_desc.inputLayout.num_bindings = (int)m_input_bindings.size();

			// create the state
			m_pso = m_renderer->createPipelineState(pso_desc);

			if (!m_pso) {
				LOG(LOG_ERROR) << "Shader::finalize: failed to create pipeline state object";
			}
		}
	}

	void Shader::setMatBufLayout(std::vector<MatBufferLayoutElement>& layout)
	{
		m_matbuf_layout = layout; 
		m_matbuf_size = 0; 
		m_matbuf_layout_map.clear();
		for (size_t i = 0; i < m_matbuf_layout.size(); i++) {
			m_matbuf_size += layout[i].size;
			m_matbuf_layout_map[layout[i].name] = static_cast<int>(i);
		}
	}

	void Shader::setMatBufLayoutVS(std::vector<MatBufferLayoutElement>& layout)
	{
		m_matbuf_layout_vs = layout;
		m_matbuf_size_vs = 0;
		m_matbuf_layout_map_vs.clear();
		for (size_t i = 0; i < m_matbuf_layout_vs.size(); i++) {
			m_matbuf_size_vs += layout[i].size;
			m_matbuf_layout_map_vs[layout[i].name] = static_cast<int>(i);
		}
	}
	
	void Shader::activate()
	{
		if (!m_pso) {
			if (!m_activate_error_logged) {
				LOG(LOG_ERROR) << "Shader::activate: no valid pipeline state, skipping activation";
				m_activate_error_logged = true;
			}
			return;
		}
		m_grahpics_core->get_renderer()->setPipelineState(m_pso.get());
		m_grahpics_core->get_renderer()->setSampler(0, m_sampler_state, Hippios::ShaderStage::Fragment);
	}

}