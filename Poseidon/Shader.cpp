#include "Shader.h"

namespace Poseidon
{
	Shader::Shader(Hippios::Renderer* renderer) :
		m_renderer(renderer),
		m_vshader(nullptr),
		m_pshader(nullptr),
		m_vertex_type(1)
	{
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
}