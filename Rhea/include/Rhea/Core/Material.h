/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_MATERIAL
#define RHEA_MATERIAL

#include "Rhea/Unit.h"
#include <Math/Vector3.h>
#include <Math/Vector4.h>
#include <Math/Vector2.h>
#include <Math/Color.h>

namespace rhea
{
	class Application;
	class ImageResource;
	class TextResource;
	class Shader;

	class Material : public Unit
	{
		MECLASS(Material, Unit)
	public:
		Material(std::string name) :
			Material(name, "//meworla/material")
		{}
		Material(std::string name, std::string type_name) :
			m_shader(nullptr),
			Unit(name, type_name)
		{}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values) override;
		std::vector<Unit::ResourceReference> getReferences();

		void ready() override;

		// get textures
		std::vector<ImageResource*>& getTextures() { return m_textures; }

		// set texture to specific slot
		void setTexture(int slot, ImageResource* res);
		void setShaderId(std::string shader_id) { m_shader_id = std::move(shader_id); }
		std::string getShaderId() const { return m_shader_id; }

		// set shader resource
		void setShader(Shader* shader);

		// get shader resource
		Shader* getShader() { return m_shader; }

		// set / get material properties

		// set color for id
		void setColor(int propertyID, math::Color color) {
			if (propertyID < 0) return;
			if (propertyID >= m_colors.size()) m_colors.resize(propertyID + 1, math::Color(0, 0, 0, 0));
			m_colors[propertyID] = color;
		}

		// get color for id
		math::Color getColor(int propertyID) { if (propertyID >= 0 && propertyID < m_colors.size()) return m_colors[propertyID]; else return math::Color(0, 0, 0, 0); }
		// set float2 for id
		void setFloat2(int propertyID, math::Vector2 value) {
			if (propertyID < 0) return;
			if (propertyID >= m_float2s.size()) m_float2s.resize(propertyID + 1, math::Vector2(0, 0));
			m_float2s[propertyID] = value;
		}
		// get float2 for id
		math::Vector2 getFloat2(int propertyID) { if (propertyID >= 0 && propertyID < m_float2s.size()) return m_float2s[propertyID]; else return math::Vector2(0, 0); }

		// set a float value by id
		void setFloat(int propertyID, float value) {
			if (propertyID < 0) return;
			if (propertyID >= m_floats.size()) m_floats.resize(propertyID + 1, 0.0f);
			m_floats[propertyID] = value;
		}
		// get a float value by id
		float getFloat(int propertyID) { if (propertyID >= 0 && propertyID < m_floats.size()) return m_floats[propertyID]; else return 0.0f; }

		// set an int value by id
		void setInt(int propertyID, int value) {
			if (propertyID < 0) return;
			if (propertyID >= m_ints.size()) m_ints.resize(propertyID + 1, 0);
			m_ints[propertyID] = value;
		}
		// get an int value by id
		int getInt(int propertyID) { if (propertyID >= 0 && propertyID < m_ints.size()) return m_ints[propertyID]; else return 0; }

		// set color for id
		void setColorVS(int propertyID, math::Vector3 color) {
			if (propertyID < 0) return;
			if (propertyID >= m_colors_vs.size()) m_colors_vs.resize(propertyID + 1, math::Color(0, 0, 0, 0));
			m_colors_vs[propertyID] = math::Color(color.x, color.y, color.z, 1.0f);
		}
		void setColorVS(int propertyID, math::Vector4 color) {
			if (propertyID < 0) return;
			if (propertyID >= m_colors_vs.size()) m_colors_vs.resize(propertyID + 1, math::Color(0, 0, 0, 0));
			m_colors_vs[propertyID] = math::Color(color.x, color.y, color.z, color.w);
		}

		// get color for id
		math::Color getColorVS(size_t propertyID) { if (propertyID >= 0 && propertyID < m_colors_vs.size()) return m_colors_vs[propertyID]; else return math::Color(0, 0, 0, 0); }
		// set float2 for id
		void setFloat2VS(size_t propertyID, math::Vector2 value) {
			if (propertyID < 0) return;
			if (propertyID >= m_float2s_vs.size()) m_float2s_vs.resize(propertyID + 1, math::Vector2(0, 0));
			m_float2s_vs[propertyID] = value;
		}
		// get float2 for id
		math::Vector2 getFloat2VS(size_t propertyID) { if (propertyID >= 0 && propertyID < m_float2s_vs.size()) return m_float2s_vs[propertyID]; else return math::Vector2(0, 0); }

		// set a float value by id
		void setFloatVS(size_t propertyID, float value) {
			if (propertyID < 0) return;
			if (propertyID >= m_floats_vs.size()) m_floats_vs.resize(propertyID + 1, 0.0f);
			m_floats_vs[propertyID] = value;
		}
		// get a float value by id
		float getFloatVS(size_t propertyID) { if (propertyID >= 0 && propertyID < m_floats_vs.size()) return m_floats_vs[propertyID]; else return 0.0f; }

		// set an int value by id
		void setIntVS(size_t propertyID, int value) {
			if (propertyID < 0) return;
			if (propertyID >= m_ints_vs.size()) m_ints_vs.resize(propertyID + 1, 0);
			m_ints_vs[propertyID] = value;
		}
		// get an int value by id
		int getIntVS(size_t propertyID) { if (propertyID >= 0 && propertyID < m_ints_vs.size()) return m_ints_vs[propertyID]; else return 0; }

	protected:
		std::string m_shader_id;
		Shader* m_shader;
		std::vector<ImageResource*> m_textures;
		std::vector<math::Color> m_colors;
		std::vector<math::Vector2> m_float2s;
		std::vector<float> m_floats;
		std::vector<int> m_ints;
		std::vector<math::Color> m_colors_vs;
		std::vector<math::Vector2> m_float2s_vs;
		std::vector<float> m_floats_vs;
		std::vector<int> m_ints_vs;
	};
}

#endif // !RHEA_MATERIAL
