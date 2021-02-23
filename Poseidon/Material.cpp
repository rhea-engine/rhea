#include "Material.h"
#include "Shader.h"
#include <cassert>

namespace Poseidon
{
	Material::Material(Hippios::Renderer* renderer) :
		m_renderer(renderer),
		m_shader(nullptr),
		m_topology(4),
		m_alpha_blending(false)
	{}

	Material::~Material()
	{
		if (m_shader)
			delete m_shader;
	}

	void Material::activate()
	{		
		assert(m_shader != nullptr);
		if (!m_shader)
			return;
		m_renderer->setAlphaBlend(m_alpha_blending);
		m_renderer->setVertexShader(m_shader->getVShader(), m_topology);
		m_renderer->setPixelShader(m_shader->getPShader());
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