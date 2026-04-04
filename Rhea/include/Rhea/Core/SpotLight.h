#ifndef RHEA_SPOTLIGHT_H
#define RHEA_SPOTLIGHT_H

#include "PointLight.h"

namespace rhea
{
	class SpotLight : public PointLight
	{
		MECLASS(SpotLight, PointLight)
	public:
		SpotLight(std::string name, std::string type) :
			PointLight(name, type)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		// get angle
		float getConeAngle() { return m_angle; }

	protected:

		float m_angle = 0.0f;
	};
}
#endif // !RHEA_SPOTLIGHT_H
