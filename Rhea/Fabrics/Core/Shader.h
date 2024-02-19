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

		// input handling
		bool usesWVP() { return m_wvp; }
		void setWVP(bool value) { m_wvp = value; }	

		// vertex layout
		std::vector<VertexAttribute>& getVertexLayout() { return m_vertex_layout; }

		// get input slot id by name
		int getInputSlotId(std::string name) { if (m_input_slots.find(name) != m_input_slots.end()) return m_input_slots[name]; else return -1; }

		// get max input slot
		int getNumInputSlots() { return m_num_input_slots; }

	protected:
		std::string m_stencil_test;
		bool m_alpha_blending;
		bool m_wvp;
		// vertex layout as a vector of element length
		std::vector<VertexAttribute> m_vertex_layout;
		// material inpout slot ids
		std::map<std::string, int> m_input_slots;
		int m_num_input_slots;
	};
}

#endif // !RHEA_SHADER_H
