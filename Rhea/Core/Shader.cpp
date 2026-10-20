/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Shader.h>


namespace rhea
{
	void Shader::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		if (m_values.find("depth_stencil_state") != m_values.end()) {
			m_depth_stencil_state = m_values["depth_stencil_state"].toString();
		}
		else {
			m_depth_stencil_state = "default";
		}

		if (m_values.find("alpha_blending") != m_values.end()) {
			m_alpha_blending = m_values["alpha_blending"];
		}

		if (m_values.find("depth_test") != m_values.end()) {
			m_depth_test = m_values["depth_test"];
		}
		if (m_values.find("depth_write") != m_values.end()) {
			m_depth_write = m_values["depth_write"];
		}

		// vertex type
		std::string vertex_layout_name = getValueString("input", "short");
		if (vertex_layout_name == "short") {
			m_vertex_layout.push_back({ "POSITION", VertexAttributeType::POSITION, 4, 3, 12, 0, 0 });
			m_vertex_layout.push_back({ "TEXCOORD",VertexAttributeType::TEXCOORD, 4, 2, 8, 12, 2 });
		}
		else if (vertex_layout_name == "standard") {
			// POSITION: vec3 (3 floats), offset 0
			m_vertex_layout.push_back({ "POSITION", VertexAttributeType::POSITION, 4, 3, 12, 0, 0 });
			// NORMAL: vec3 (3 floats), offset 12
			m_vertex_layout.push_back({ "NORMAL", VertexAttributeType::NORMAL, 4, 3, 12, 12, 1 });
			// TEXCOORD: vec2 (2 floats), offset 24
			m_vertex_layout.push_back({ "TEXCOORD", VertexAttributeType::TEXCOORD, 4, 2, 8, 24, 2 });
			// TANGENT: vec3 (3 floats), offset 32
			m_vertex_layout.push_back({ "TANGENT", VertexAttributeType::TANGENT, 4, 3, 12, 32, 3 });
		}
		else if (vertex_layout_name == "skeletal") {
			// POSITION: vec3 (3 floats), offset 0
			m_vertex_layout.push_back({ "POSITION", VertexAttributeType::POSITION, 4, 3, 12, 0, 0 });
			// NORMAL: vec3 (3 floats), offset 12
			m_vertex_layout.push_back({ "NORMAL", VertexAttributeType::NORMAL, 4, 3, 12, 12, 1 });
			// TEXCOORD: vec2 (2 floats), offset 24
			m_vertex_layout.push_back({ "TEXCOORD", VertexAttributeType::TEXCOORD, 4, 2, 8, 24, 2 });
			// TANGENT: vec3 (3 floats), offset 32
			m_vertex_layout.push_back({ "TANGENT", VertexAttributeType::TANGENT, 4, 3, 12, 32, 3 });
			// JOINTS_0: uvec4 (4 unsigned integers for joint indices), offset 44
			m_vertex_layout.push_back({ "JOINTS", VertexAttributeType::JOINTS, 2, 4, 8, 44, 4 });
			// WEIGHTS_0: vec4 (4 floats for joint weights), offset 60
			m_vertex_layout.push_back({ "WEIGHTS", VertexAttributeType::WEIGHTS, 4, 4, 16, 52, 5 });
		}
		else if (vertex_layout_name == "color") {
			m_vertex_layout.push_back({ "POSITION", VertexAttributeType::POSITION, 4, 3, 12, 0, 0 });
			m_vertex_layout.push_back({ "COLOR", VertexAttributeType::COLOR, 4, 4, 16, 12, 1 });
		}
		else /* if (vertex_layout_name == "default") */ {
			m_vertex_layout.push_back({ "POSITION", VertexAttributeType::POSITION, 4, 3, 12, 0, 0 });
			m_vertex_layout.push_back({ "NORMAL", VertexAttributeType::NORMAL, 4, 3, 12, 12, 1 });
			m_vertex_layout.push_back({ "TEXCOORD", VertexAttributeType::TEXCOORD, 4, 2, 8, 24, 2 });
		}
		
		// setup material input slots
		std::string light_type = getValueString("light_type", "none");		
		if (light_type == "vertex") {
			m_property_names = { "AMBIENT", "DIFFUSE", "SPECULAR", "SHININESS", "EMISSIVE" };
			m_input_slots["AMBIENT"] = 0;
			m_input_slots["DIFFUSE"] = 1;
			m_input_slots["SPECULAR"] = 2;
			m_input_slots["SHININESS"] = 3;
			m_input_slots["EMISSIVE"] = 4;
			m_num_input_slots = 5;
		}
		else if (m_values.find("material_slots_ps") != m_values.end() && m_values["material_slots_ps"].isArray()) {
			for (int i = 0; i < m_values["material_slots_ps"].size(); i++) {
				m_property_names.push_back(m_values["material_slots_ps"][i].toString());
				m_input_slots[m_values["material_slots_ps"][i].toString()] = i;
				m_num_input_slots = i + 1;
			}
		}
		if (m_values.find("material_slots_vs") != m_values.end() && m_values["material_slots_vs"].isArray()) {
			for (int i = 0; i < m_values["material_slots_vs"].size(); i++) {
				m_property_names_vs.push_back(m_values["material_slots_vs"][i].toString());
				// TODO support other types than float2
				m_input_slots_vs[m_values["material_slots_vs"][i].toString()] = {(size_t)i, InputType::kVector2};
				m_num_input_slots_vs = static_cast<size_t>(i) + 1;
			}
		}
		
		// setup texture slots
		std::string texture_type = getValueString("texture_type", "none");
		if (texture_type == "standard") {
			m_texture_slot_descs.push_back({ "BASECOLOR", "BaseColorTexture", 0 });
			m_texture_slot_descs.push_back({ "NORMALMAP", "NormalMap", 1 });
		}
		else if (texture_type == "terrain") {
			m_texture_slot_descs.push_back({ "BLENDMAP", "Texture0", 0 });
			m_texture_slot_descs.push_back({ "TEXTURE1", "Texture1", 1 });
			m_texture_slot_descs.push_back({ "TEXTURE2", "Texture2", 2 });
			m_texture_slot_descs.push_back({ "TEXTURE3", "Texture3", 3 });
			m_texture_slot_descs.push_back({ "TEXTURE4", "Texture4", 4 });
		}
		else if (texture_type == "skybox") {
			m_texture_slot_descs.push_back({ "BASECOLOR", "SkyMap", 0 });
		}
		else {
			// default has at least the base color
			m_texture_slot_descs.push_back({ "BASECOLOR", "BaseColorTexture", 0 });
		}
		m_num_texture_slots = (int)m_texture_slot_descs.size();
	}	

	TextResource* rhea::Shader::getShaderCode(ShaderBackend backend, ShaderStage stage)
	{
		std::string key;

		if (backend == ShaderBackend::D3D11) {
			return m_app->getUnitTree()->resolve(m_shader_code_hlsl);
		}
		else if (backend == ShaderBackend::OpenGL) {
			if (stage == ShaderStage::Vertex) {
				return m_app->getUnitTree()->resolve(m_shader_code_glsl_vs);
			} else {
				return m_app->getUnitTree()->resolve(m_shader_code_glsl_fs);
			}
		}

		return m_app->getUnitTree()->resolve(m_code);		
	}

	std::string rhea::Shader::getEntryPoint(ShaderBackend backend, ShaderStage stage) const
	{		
		if (backend == ShaderBackend::D3D11) {
			if (stage == ShaderStage::Vertex)
				return getValueString("vs_entry", "VShader");
			else
				return getValueString("ps_entry", "PShader");
		}
		return "";
	}

	bool Shader::hasShaderCode(ShaderBackend backend, ShaderStage stage) const
	{
		std::string key;
		if (backend == ShaderBackend::D3D11) {
			return !m_shader_code_hlsl.empty();
		}
		else if (backend == ShaderBackend::OpenGL) {
			if (stage == ShaderStage::Vertex) {
				return !m_shader_code_glsl_vs.empty();
			} else {
				return !m_shader_code_glsl_fs.empty();
			}
		}
		else {
			return !m_code.empty();
		}
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

	void Shader::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "code_hlsl", &Shader::m_shader_code_hlsl, "//meworla/text", ResourceUsage::Load);
		registerResourceProperty(type, "code_glsl_vs", &Shader::m_shader_code_glsl_vs, "//meworla/text", ResourceUsage::Load);
		registerResourceProperty(type, "code_glsl_fs", &Shader::m_shader_code_glsl_fs, "//meworla/text", ResourceUsage::Load);
		registerResourceProperty(type, "code", &Shader::m_code, "//meworla/text", ResourceUsage::Load);
	}
}