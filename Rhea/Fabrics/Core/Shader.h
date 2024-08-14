#ifndef RHEA_SHADER_H
#define RHEA_SHADER_H

#include <Rhea/Unit.h>
#include <Rhea/Resources/TextResource.h>
#include <Rhea/Resources/VertexAttribute.h>

namespace Rhea
{
	class Shader : public Unit
	{
		MECLASS(Shader, Unit)
	public:
		Shader(std::string name, std::string typename) :
			m_alpha_blending(false),
			m_wvp(false),
			m_num_input_slots(0),
			m_num_texture_slots(0),
			m_vertex_size(-1),
			m_vertex_elements(-1),
			Unit(name, "//meworla/shader")
		{}

		// setup
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
		std::vector<Unit::Export> getReferences();

		TextResource* getShaderCode();

		// stencil testing		
		void setStencilTest(std::string test_name) { m_stencil_test = test_name; }
		std::string getStencilTest() { return m_stencil_test; }

		// alpha blending
		bool getAlphaBlending() { return m_alpha_blending; }
		void setAlphaBlending(bool value) { m_alpha_blending = value; }

		// vertex layout
		std::vector<VertexAttribute>& getVertexLayout() { return m_vertex_layout; }

		// get input slot id by name
		int getInputSlotId(std::string name) { if (m_input_slots.find(name) != m_input_slots.end()) return m_input_slots[name]; else return -1; }

		// get max input slot
		int getNumInputSlots() const { return m_num_input_slots; }

		// get texture input slot id by name
		int getTextureSlotId(std::string name) { if (m_texture_slots.find(name) != m_texture_slots.end()) return m_texture_slots[name]; else return -1; }

		// get max texture input slot
		int getNumTextureSlots() const { return m_num_texture_slots; }

		// get vertex size for input layout
		int getVertexSize();

		// get vertex element count for input layout
		int getVertexElements();

	protected:
		std::string m_stencil_test;
		bool m_alpha_blending;
		bool m_wvp;
		// vertex layout as a vector of element length
		std::vector<VertexAttribute> m_vertex_layout;
		// material inpout slot ids
		std::map<std::string, int> m_input_slots;
		int m_num_input_slots;
		// texture input slots
		std::map<std::string, int> m_texture_slots;
		int m_num_texture_slots;

		// vertex size cache
		int m_vertex_size;
		// vertex element count cache
		int m_vertex_elements;
	};
}

#endif // !RHEA_SHADER_H
