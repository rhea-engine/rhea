#include "Material.h"

namespace Poseidon
{
	Material::Material(Hippios::Renderer* renderer) : m_renderer(renderer)
	{}

	bool Material::loadFromFile(std::string filename, char* shader_type, char* entry_point)
	{
		return m_renderer->addMaterial(filename, shader_type, entry_point, m_type);
	}

	void Material::initBuffer(size_t size)
	{
		m_renderer->initGPUBuffer(size);
	}

	// set the vertex type
	void Material::setType(int type)
	{
		m_type = type;
	}
}