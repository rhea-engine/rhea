#ifndef RHEA_LIGHTING_H
#define RHEA_LIGHTING_H

#include <Math/Quaternion.h>

#include <Rhea/Unit.h>

namespace rhea
{
	class Lighting : public Unit
	{
		MECLASS(Lighting, Unit)
	public:
		Lighting(std::string name) :
			Lighting(name, "//meworla/lighting")
		{}
		Lighting(std::string name, std::string type_name) :
			Unit(name, type_name)
		{}

		// setup
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// get ambient value
		Math::Quaternion getAmbient() { return m_ambient; }
		// set ambient value
		void setAmbient(Math::Quaternion ambient) { m_ambient = ambient; }

	protected:
		Math::Quaternion m_ambient;
	};
}
#endif // !RHEA_LIGHTING_H
