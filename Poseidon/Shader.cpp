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
		m_cull_mode(true)
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
		m_vshader = m_renderer->addMaterial(filename, "vs", vs_entry_point, m_vertex_type);
		m_pshader = m_renderer->addMaterial(filename, "ps", ps_entry_point, m_vertex_type);

		return (m_pshader == nullptr || m_vshader == nullptr);
	}

	bool Shader::loadFromBuffer(unsigned char* buf, size_t buflen, char* vs_entry_point, char* ps_entry_point)
	{
		m_vshader = m_renderer->addMaterial(buf, buflen, "vs", vs_entry_point, m_vertex_type);
		m_pshader = m_renderer->addMaterial(buf, buflen, "ps", ps_entry_point, m_vertex_type);

		return (m_pshader == nullptr || m_vshader == nullptr);
	}

	void Shader::finalize()
	{
		if (!m_pso) {
			Core::PipelineProperties props;
			props.topology = m_topology_type;
			props.alpha_blending = m_alpha_blending;
			props.shader = this;
			props.stencil_test = m_stencil_test;
			props.cull_face_mode = m_cull_mode;
			m_pso = m_grahpics_core->CreateOrGetPso(props);
		}
	}

	void Shader::activate()
	{
		assert(m_pso != nullptr);
		m_grahpics_core->get_renderer()->setPipelineState(m_pso);
	}

}