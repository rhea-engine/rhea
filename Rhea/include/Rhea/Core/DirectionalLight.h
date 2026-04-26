#ifndef RHEA_DIRECTIONAL_LIGHT_H
#define RHEA_DIRECTIONAL_LIGHT_H

#include "Light.h"

namespace rhea
{
	class DirectionalLight : public Light
	{
		MECLASS(DirectionalLight, Light)
	public:
		DirectionalLight(std::string name, std::string type) :
			Light(name, type)
		{}
		
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void setDirection(math::Vector3 direction) { m_direction = direction; }
		math::Vector3 getDirection() { return m_direction; }

	protected:
		math::Vector3 m_direction;		
	};
}
#endif // !RHEA_DIRECTIONAL_LIGHT_H
