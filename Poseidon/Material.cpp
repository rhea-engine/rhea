#include "Material.h"

namespace Poseidon
{
	Material::Material(Hippios::Renderer* renderer) :
		m_renderer(renderer),
		m_vshader(nullptr),
		m_pshader(nullptr),
		m_cb_per_object_buffer(nullptr)
	{}

	Material::~Material()
	{
		if (m_vshader)
			delete m_vshader;
		if (m_pshader)
			delete m_pshader;
	}

	bool Material::loadFromFile(std::string filename, char* vs_entry_point, char* ps_entry_point)
	{
		m_vshader = m_renderer->addMaterial(filename, "vs", vs_entry_point, m_type);
		m_pshader = m_renderer->addMaterial(filename, "ps", ps_entry_point, m_type);

		return (m_pshader == nullptr || m_vshader == nullptr);
	}

	void Material::initBuffer(size_t size)
	{
		m_cb_per_object_buffer = m_renderer->initGPUBuffer(size);
	}

	// set the vertex type
	void Material::setType(int type)
	{
		m_type = type;
	}
	
	void Material::activate()
	{
		if (!m_type)
			m_renderer->setVertexShader(m_vshader, 2);
		else
			m_renderer->setVertexShader(m_vshader, 4);
		m_renderer->setPixelShader(m_pshader);
		m_renderer->setGPUBuffer(m_cb_per_object_buffer);
	}
}