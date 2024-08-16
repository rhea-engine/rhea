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

		// vertex type
		std::string vertex_layout_name = getValue("input", "short");
		if (vertex_layout_name == "short") {
			m_vertex_layout.push_back({ "POSITION", VertexAttributeType::POSITION, 4, 3, 12, 0 });
			m_vertex_layout.push_back({ "TEXCOORD",VertexAttributeType::TEXCOORD, 4, 2, 8, 12 });
		}
		else if (vertex_layout_name == "standard") {
			m_vertex_layout.push_back({ "POSITION", VertexAttributeType::POSITION, 4, 3, 12, 0 });
			m_vertex_layout.push_back({ "NORMAL", VertexAttributeType::NORMAL, 4, 3, 12, 12 });
			m_vertex_layout.push_back({ "TEXCOORD", VertexAttributeType::TEXCOORD, 4, 2, 8, 24 });
			m_vertex_layout.push_back({ "TANGENT", VertexAttributeType::TEXCOORD, 4, 2, 12, 32 });
		}
		else /* if (vertex_layout_name == "default") */ {
			m_vertex_layout.push_back({ "POSITION", VertexAttributeType::POSITION, 4, 3, 12, 0 });
			m_vertex_layout.push_back({ "NORMAL", VertexAttributeType::NORMAL, 4, 3, 12, 12 });
			m_vertex_layout.push_back({ "TEXCOORD", VertexAttributeType::TEXCOORD, 4, 2, 8, 24 });
		}

		// setup material input slots
		std::string light_type = getValue("light_type", "none");
		if (light_type == "vertex") {
			m_input_slots["AMBIENT"] = 0;
			m_input_slots["DIFFUSE"] = 1;
			m_input_slots["SPECULAR"] = 2;
			m_input_slots["SHININESS"] = 3;
			m_input_slots["EMISSIVE"] = 4;
			m_num_input_slots = 5;
		}

		// setup texture slots
		std::string texture_type = getValue("texture_type", "none");
		if (texture_type == "standard") {
			m_texture_slots["BASECOLOR"] = 0;
			m_texture_slots["NORMAL"] = 1;
		}
		else {
			// default has at least the base color
			m_texture_slots["BASECOLOR"] = 0;
		}
		m_num_texture_slots = (int)m_texture_slots.size();
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

	int Shader::getVertexSize()
	{
		// only compute if not cached
		if (m_vertex_size < 0)
		{
			m_vertex_size = 0;
			m_vertex_elements = 0;
			for (auto& attribute : m_vertex_layout) {
				m_vertex_size += attribute.size;
				m_vertex_elements += attribute.element_count;
			}
		}
			
		return m_vertex_size;
	}

	int Shader::getVertexElements()
	{
		// only compute if not cached
		if (m_vertex_size < 0) {
			int dummy = getVertexSize();
		}
		return m_vertex_elements;
	}
}