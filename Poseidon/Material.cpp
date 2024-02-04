#include "Material.h"
#include "Shader.h"
#include <cassert>
#include <Core.h>

namespace Poseidon
{
	Material::Material(Core* graphics_core) :
		m_grahpics_core(graphics_core),
		m_shader(nullptr),
		m_topology(4)
	{}

	Material::~Material()
	{
		if (m_shader)
			delete m_shader;
	}

	void Material::activate()
	{		
		// activet the shader (the pso)
		m_shader->activate();
	}

	void Material::addTexture(Texture* texture)
	{
		m_textures.push_back(texture);
	}

	Hippios::GPUProgram* Material::getVShader()
	{
		if (!m_shader)
			return nullptr;
		return m_shader->getVShader();
	}

	Hippios::GPUProgram* Material::getPShader()
	{
		if (!m_shader)
			return nullptr;
		return m_shader->getPShader();
	}
}