#ifndef RHEA_POINTLIGHT_H
#define RHEA_POINTLIGHT_H

#include "Light.h"

namespace rhea
{
	class PointLight : public Light
	{
		MECLASS(PointLight, Light)
	public:
		PointLight(std::string name, std::string type) :
			Light(name, type)
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		Math::Vector3 getAttenuation() { return m_attenuation; }

	protected:
		Math::Vector3 m_attenuation;
	};
}
#endif // !RHEA_POINTLIGHT_H
