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
#include <cctype>

namespace rhea
{
	void Material::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		if (m_shader_ref.empty())
			m_shader_ref.setId(getValueString("shader", "//rhea/shaders/standard"));

		// add all referenced textures (starting with "texture_") and value is not empty
		for (const auto& [key, value] : m_values) {
			if (key.compare(0, 8, "texture_") != 0 || value.toString().empty())
				continue;

			std::string texture_key = key;
			std::transform(
				texture_key.begin(), texture_key.end(), texture_key.begin(),
				[](unsigned char c) { return static_cast<char>(std::tolower(c)); });
			if (!texture_key.empty()) {
				m_texture_refs[texture_key].setId(value.toString());
				m_texture_refs[texture_key].setBasePath(m_attributes["basepath"]);
			}
		}
	}

	void Material::initializeType(simpleparser::Type* type)
	{
		registerResourceProperty(type, "shader", &Material::m_shader_ref, "//meworla/shader", ResourceUsage::Load);
	}

	void Material::resolveReferences(const std::string& context_root, const std::string& source_root)
	{
		Unit::resolveReferences(context_root, source_root);
		const ResourceContext context{ context_root, source_root };
		for (auto& [key, value] : m_texture_refs) {
			if (!value.empty())
				normalizeReference(value, context);
		}
	}

	std::vector<Unit::ResourceReference> Material::getReferences()
	{
		std::vector<ResourceReference> refs = Unit::getReferences();
		
		// add all referenced textures 
		for (const auto& [key, value] : m_texture_refs) {
			refs.push_back({ key, "//meworla/texture", value.id()});
		}
		return refs;
	}

	Material* Material::Clone() const
	{
		auto* result = new Material(m_pathname);

		result->setup(m_app, m_attributes, m_values);
		result->setOrigin(ComponentOrigin::Runtime);

		result->m_shader_ref = m_shader_ref;
		result->m_texture_refs = m_texture_refs;
		result->m_shader = m_shader;

		result->m_textures = m_textures;

		result->m_colors = m_colors;
		result->m_float2s = m_float2s;
		result->m_floats = m_floats;
		result->m_ints = m_ints;

		result->m_colors_vs = m_colors_vs;
		result->m_float2s_vs = m_float2s_vs;
		result->m_floats_vs = m_floats_vs;
		result->m_ints_vs = m_ints_vs;

		return result;
	}

	void Material::ready()
	{
		Unit::ready();

		if (!m_shader_ref.empty()) {
			// find the shader by id
			Shader* shader = m_app->getUnitTree()->resolve(m_shader_ref);
			if (!shader) {
				LOG(LOG_ERROR) << "shader not found: " << m_shader_ref.id();
				return;
			}

			setShader(shader);
		}

		// try to fill input slots of shader
		
		// first ps stage
		if (!m_shader) {
			LOG(LOG_ERROR) << "shader not set for material: " << getPathName();
			return;
		}
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


		// handle textures

		for (const auto& desc : m_shader->getTextureSlots()) {
			std::string key = "texture_" + desc.propertyName;
			std::transform(key.begin(), key.end(), key.begin(), ::tolower);

			const auto texture_it = m_texture_refs.find(key);
			if (texture_it == m_texture_refs.end())
				continue;

			const auto& reference = texture_it->second;
			auto* texture = m_app->getUnitTree()->resolve(reference);

			if (texture)
				setTexture(desc.slot, texture);
			else
				LOG(LOG_WARNING) << "Material texture not found: " << reference.id();
		}
	}

	void Material::setShader(Shader* shader)
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

	void Material::setTexture(int slot, ImageResource* res)
	{ 
		if (slot < 0)
			return; 
		
		const size_t index = static_cast<size_t>(slot);

		if (index >= m_textures.size() && res) 
			m_textures.resize(index + 1, nullptr); 
		
		m_textures[index] = res;
	}

}
