#include "Material.h"
#include "Shader.h"
#include <cassert>
#include <Core.h>

namespace Poseidon
{
	Material::Material(Core* graphics_core) :
		m_grahpics_core(graphics_core),
		m_shader(nullptr),
		m_topology(4),
		m_alpha_blending(false),
		m_pso(nullptr),
		m_stencil_test("default")
	{}

	Material::~Material()
	{
		if (m_shader)
			delete m_shader;
	}

	void Material::finalize()
	{
		if (!m_pso) {
			Core::PipelineProperties props;
			props.topology = m_topology;
			props.alpha_blending = m_alpha_blending;
			props.shader = m_shader;
			props.stencil_test = m_stencil_test;
			m_pso = m_grahpics_core->CreateOrGetPso(props);
		}
	}

	void Material::activate()
	{		
		assert(m_pso != nullptr);
		m_grahpics_core->get_renderer()->setPipelineState(m_pso);
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