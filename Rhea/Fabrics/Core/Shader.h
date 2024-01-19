#ifndef RHEA_SHADER_H
#define RHEA_SHADER_H

#include <Rhea/Unit.h>
#include <Rhea/Resources/TextResource.h>

namespace Rhea
{
	class Shader : public Unit
	{
		MECLASS(Shader, Unit)
	public:
		Shader(std::string name, std::string typename) :
			m_alpha_blending(false),
			m_wvp(false),
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

	protected:
		std::string m_stencil_test;
		bool m_alpha_blending;
		bool m_wvp;
	};
}

#endif // !RHEA_SHADER_H
