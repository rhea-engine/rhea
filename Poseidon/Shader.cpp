#include "Shader.h"
#include <cassert>

namespace Poseidon
{
	Shader::Shader(Core* core) :
		m_grahpics_core(core),
		m_vshader(nullptr),
		m_pshader(nullptr),
		m_vertex_type(1),
		m_alpha_blending(false),
		m_pso(nullptr),
		m_stencil_test("default"),
		m_topology_type(0),
		m_cull_mode(true),
		m_matbuf_size(0),
		m_material_buffer(nullptr),
		m_matbuf_layout(),
		m_renderer(nullptr)
	{
		m_renderer = m_grahpics_core->get_renderer();
	}

	Shader::~Shader()
	{
		if (m_vshader)
			delete m_vshader;
		if (m_pshader)
			delete m_pshader;
	}

	bool Shader::loadFromFile(std::string filename, char* vs_entry_point, char* ps_entry_point)
	{
		m_vshader = m_renderer->addMaterial(filename, "vs", vs_entry_point);
		m_pshader = m_renderer->addMaterial(filename, "ps", ps_entry_point);

		return (m_pshader == nullptr || m_vshader == nullptr);
	}

	bool Shader::loadFromBuffer(unsigned char* buf, size_t buflen, char* vs_entry_point, char* ps_entry_point)
	{
		m_vshader = m_renderer->addMaterial(buf, (int)buflen, "vs", vs_entry_point);
		m_pshader = m_renderer->addMaterial(buf, (int)buflen, "ps", ps_entry_point);

		return (m_pshader == nullptr || m_vshader == nullptr);
	}

	void Shader::finalize()
	{
		if (!m_pso) {

			Hippios::PipelineStateObjectDesc pso_desc;

			// stencil state
			pso_desc.depthStencilState.name = m_stencil_test;
			// defaults
			pso_desc.blendState.renderTarget[0].src_blend = Hippios::BlendFactor::BLEND_ONE;
			pso_desc.blendState.renderTarget[0].dest_blend = Hippios::BlendFactor::BLEND_INV_SRC_ALPHA;
			pso_desc.blendState.renderTarget[0].blend_op = Hippios::BlendOp::BLEND_ADD;
			pso_desc.blendState.renderTarget[0].src_blend_alpha = Hippios::BlendFactor::BLEND_ONE;
			pso_desc.blendState.renderTarget[0].dest_blend_alpha = Hippios::BlendFactor::BLEND_ZERO;
			pso_desc.blendState.renderTarget[0].blend_op_alpha = Hippios::BlendOp::BLEND_ADD;
			pso_desc.blendState.renderTarget[0].render_target_write_mask = 0x0f;
			if (m_alpha_blending)
				pso_desc.blendState.renderTarget[0].blend_enable = true;

			// cull mode in raster state
			if (!m_cull_mode)
				pso_desc.rasterizerState.cull_mode = Hippios::CullMode::NONE;

			// shader programs
			pso_desc.shaderProgram.vertex_shader = getVShader();
			pso_desc.shaderProgram.pixel_shader = getPShader();

			// layout

			Hippios::InputLayoutElement elements_pos_uv[] = {
				{ "POSITION", 0, Hippios::InputLayoutFormat::R32G32B32_FLOAT, 0, 0 },
				{ "TEXCOORD", 0, Hippios::InputLayoutFormat::R32G32_FLOAT, 0, 12 }
			};
			Hippios::InputLayoutElement elements_pos_norm_uv[] = {
				{ "POSITION", 0, Hippios::InputLayoutFormat::R32G32B32_FLOAT, 0, 0 },
				{ "NORMAL", 0, Hippios::InputLayoutFormat::R32G32B32_FLOAT, 0, 12},
				{ "TEXCOORD", 0, Hippios::InputLayoutFormat::R32G32_FLOAT, 0, 24 }
			};
			Hippios::InputLayoutElement elements_pos_norm_uv_tangent[] = {
				{ "POSITION", 0, Hippios::InputLayoutFormat::R32G32B32_FLOAT, 0, 0 },
				{ "NORMAL", 0, Hippios::InputLayoutFormat::R32G32B32_FLOAT, 0, 12},
				{ "TEXCOORD", 0, Hippios::InputLayoutFormat::R32G32_FLOAT, 0, 24 },
				{ "TANGENT", 0, Hippios::InputLayoutFormat::R32G32B32_FLOAT, 0, 32 }
			};
			Hippios::InputLayoutElement elements_pos_norm_uv_tangent_skeletal[] = {
				{ "POSITION", 0, Hippios::InputLayoutFormat::R32G32B32_FLOAT, 0, 0 },
				{ "NORMAL", 0, Hippios::InputLayoutFormat::R32G32B32_FLOAT, 0, 12 },
				{ "TEXCOORD", 0, Hippios::InputLayoutFormat::R32G32_FLOAT, 0, 24 },
				{ "TANGENT", 0, Hippios::InputLayoutFormat::R32G32B32_FLOAT, 0, 32 },
				{ "JOINTS", 0, Hippios::InputLayoutFormat::R32G32B32A32_UINT, 0, 44 }, // Bone indices
				{ "WEIGHTS", 0, Hippios::InputLayoutFormat::R32G32B32A32_FLOAT, 0, 60 } // Bone weights
			};
			switch (m_vertex_type) {
			case 0: // default
				pso_desc.inputLayout.elements = &elements_pos_norm_uv[0];
				pso_desc.inputLayout.num_elements = 3;
				break;
			case 1: // short (no normals)
				pso_desc.inputLayout.elements = &elements_pos_uv[0];
				pso_desc.inputLayout.num_elements = 2;
				break;
			case 2: // color
				// TODO
			case 3:
				pso_desc.inputLayout.elements = &elements_pos_norm_uv_tangent[0];
				pso_desc.inputLayout.num_elements = 4;
				break;
			case 4:
				pso_desc.inputLayout.elements = &elements_pos_norm_uv_tangent_skeletal[0];
				pso_desc.inputLayout.num_elements = 6;
				break;
			case 5: // custom
				// TODO
			default: // default
				pso_desc.inputLayout.elements = &elements_pos_norm_uv[0];
				pso_desc.inputLayout.num_elements = 3;
			}
			switch (m_topology_type) {
			case 1:
				pso_desc.vertexLayout = Hippios::PrimitiveTopology::LineList;
				break;
			default:
				pso_desc.vertexLayout = Hippios::PrimitiveTopology::TriangleList;
			}

			// create the state
			m_pso.reset(m_renderer->createPipelineState(&pso_desc));
		}
	}

	void Shader::setMatBufLayout(std::vector<MatBufferLayoutElement>& layout)
	{
		m_matbuf_layout = layout; 
		m_matbuf_size = 0; 
		for (int i = 0; i < m_matbuf_layout.size(); i++) {
			m_matbuf_size += layout[i].size;
			m_matbuf_layout_map[layout[i].name] = i;
		}
	}

	void Shader::activate()
	{
		assert(m_pso != nullptr);
		m_grahpics_core->get_renderer()->setPipelineState(m_pso.get());
	}

}