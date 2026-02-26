#include "Rhea/Core/Material.h"
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>
#include "Rhea/Resources/TextResource.h"
#include <Rhea/Core/Shader.h>

namespace Rhea 
{
	std::vector<Unit::Export> Material::getReferences()
	{
		std::vector<Export> refs;		
		if (m_values.find("shaderpath") != m_values.end()) {			
			refs.push_back({ "shader", "//meworla/shader", m_values["shaderpath"] });
		}
		return refs;
	}

	void Material::ready()
	{
		Unit::ready();

		// get the shader
		if (m_values.find("shaderpath") == m_values.end()) {
			return;
		}
		TextResource* shader_res = static_cast<TextResource*>(m_app->getUnitTree()->find(m_values["shaderpath"], m_attributes["basepath"], false));
		if (shader_res) {
			setShader(shader_res);
		}
		else {
			LOG(LOG_ERROR) << "shader not found: " << (std::string)m_values["shaderpath"];
		}
	}

	void Rhea::Material::setShaderRes(Shader* shader)
	{
		m_shader_res = shader; 

		// fill slots for properties
		if (m_shader_res) {
			int num_slots = m_shader_res->getNumInputSlots();
			m_colors.resize(num_slots);
			m_floats.resize(num_slots);
			m_ints.resize(num_slots);

			size_t num_slots_vs = m_shader_res->getNumInputSlotsVS();
			m_colors_vs.resize(num_slots_vs);
			m_float2s_vs.resize(num_slots_vs);
			m_floats_vs.resize(num_slots_vs);
			m_ints_vs.resize(num_slots_vs);

			m_textures.resize(m_shader_res->getNumTextureSlots());
		}
	}
}
