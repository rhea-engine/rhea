#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include "Shader.h"


namespace Rhea
{
	void Shader::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		if (m_values.find("stencil_test") != m_values.end()) {
			m_stencil_test = m_values["stencil_test"];
		}
		else {
			m_stencil_test = "default";
		}

		if (m_values.find("alpha_blending") != m_values.end()) {
			m_alpha_blending = m_values["alpha_blending"];
		}
	}

	std::vector<Unit::Export> Rhea::Shader::getReferences()
	{
		std::vector<Export> refs = std::vector<Unit::Export>();		
		if (m_values.find("code") != m_values.end()) {
			refs.push_back({ "code", "//meworla/shader", m_values["code"] });
		}
		return refs;
	}

	TextResource* Rhea::Shader::getShaderCode()
	{
		if (m_values.find("code") != m_values.end()) {
			TextResource* res = doCast<TextResource>(m_app->getUnitTree()->find(m_values["code"], m_attributes["basepath"], false));
			return res;
		}
		return nullptr;
	}	
}