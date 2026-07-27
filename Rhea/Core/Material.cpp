/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Core/Material.h"
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>
#include <Rhea/Resources/ImageResource.h>
#include <Rhea/Core/Shader.h>
#include <algorithm>

namespace rhea 
{
	void Material::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		m_shader_id = getValueString("shader", "//rhea/shaders/standard");
	}

	std::vector<Unit::Export> Material::getReferences()
	{
		std::vector<Export> refs;		
		if (m_values.find("shaderpath") != m_values.end()) {			
			refs.push_back({ "shader", "//meworla/shader", m_values["shaderpath"] });
		}
		// add all referenced textures (starting with "texture_")
		for (const auto& [key, value] : m_values) {
			if (key.rfind("texture_", 0) == 0) { // starts with "texture_"
				refs.push_back({ key, "//meworla/texture", value });
			}
		}
		return refs;
	}

	void Material::ready()
	{
		Unit::ready();

		if (!m_shader && !m_shader_id.empty()) {
			// find the shader by id
			Shader* shader_res = doCast<Shader>(m_app->getUnitTree()->find(m_shader_id, m_attributes["basepath"], false));
			if (!shader_res) {
				LOG(LOG_ERROR) << "shader not found: " << m_shader_id;
			}
			else {
				setShader(shader_res);

				// try to fill input slots of shader

				// first ps stage
				auto slot_names = m_shader->getInputSlotNames();
				for (auto& slot_name : slot_names) {
					std::string key = slot_name;
					// use lowercase for key
					std::transform(key.begin(), key.end(), key.begin(), ::tolower);
					if (m_values.find(key) != m_values.end()) {
						Variant value = m_values[key];
						int inputslot_id = m_shader->getInputSlotId(slot_name);
						if (inputslot_id < 0) {
							LOG(LOG_WARNING) << "shader input slot not found for: " << slot_name;
							continue;
						}
						if (value.isArray() && (value.size() == 3 || value.size() == 4)) {
							if (value.size() == 3) {
								setColor(inputslot_id, math::Color(value[0].toFloat(), value[1].toFloat(), value[2].toFloat(), 1.0f));
							}
							else if (value.size() == 4) {
								setColor(inputslot_id, math::Color(value[0].toFloat(), value[1].toFloat(), value[2].toFloat(), value[3].toFloat()));
							}
						}
						else
							// Note: we assume that all other values are floats, this is a simplification and may not be correct in all cases
							// but needs shader update for vs stage colors
							setFloat(inputslot_id, value.toFloat());
					}
				}

				// the vs stage
				
				slot_names = m_shader->getInputSlotNamesVS();
				for (auto& slot_name : slot_names) {
					std::string key = slot_name;
					// use lowercase for key
					std::transform(key.begin(), key.end(), key.begin(), ::tolower);
					if (m_values.find(key) != m_values.end()) {
						Variant value = m_values[key];
						Shader::InputSlot* slot = m_shader->getInputSlotVS(slot_name);
						if (!slot) {
							LOG(LOG_WARNING) << "shader input slot not found for: " << slot_name;
							continue;
						}
						switch (slot->type) {
						case Shader::InputType::kVector2:
							if (value.size() == 2) {
								setFloat2VS(slot->slot_id, math::Vector2(value[0].toFloat(), value[1].toFloat()));

							}
							else {
								LOG(LOG_WARNING) << "shader input slot expects 2 values for: " << slot_name;
							}
							break;
						default:
							LOG(LOG_ERROR) << "shader input slot type not supported for: " << slot_name;
						}
					}
				}
			}

			// handle textures

			for (const auto& desc : m_shader->getTextureSlots()) {
				std::string key = "texture_" + desc.propertyName;
				std::transform(key.begin(), key.end(), key.begin(), ::tolower);

				auto it = m_values.find(key);
				if (it == m_values.end())
					continue;

				auto* texture = doCast<ImageResource>(m_app->getUnitTree()->find(it->second.toString(), m_attributes["basepath"], false));

				if (texture)
					setTexture(desc.slot, texture);
				else
					LOG(LOG_WARNING) << "Material texture not found: " << it->second.toString();
			}
		}
	}

	void rhea::Material::setShader(Shader* shader)
	{
		m_shader = shader; 

		// fill slots for properties
		if (m_shader) {
			int num_slots = m_shader->getNumInputSlots();
			m_colors.resize(num_slots);
			m_floats.resize(num_slots);
			m_ints.resize(num_slots);

			size_t num_slots_vs = m_shader->getNumInputSlotsVS();
			m_colors_vs.resize(num_slots_vs);
			m_float2s_vs.resize(num_slots_vs);
			m_floats_vs.resize(num_slots_vs);
			m_ints_vs.resize(num_slots_vs);

			m_textures.resize(m_shader->getNumTextureSlots());
		}
	}
}
