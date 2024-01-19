#include "Material.h"
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>
#include "Resources/TextResource.h"

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
}
