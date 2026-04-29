/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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

		math::Vector3 getAttenuation() { return m_attenuation; }

	protected:
		math::Vector3 m_attenuation;
	};
}
#endif // !RHEA_POINTLIGHT_H
