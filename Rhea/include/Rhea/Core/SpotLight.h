/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SPOTLIGHT_H
#define RHEA_SPOTLIGHT_H

#include "PointLight.h"

namespace rhea
{
	class SpotLight : public PointLight
	{
		RECLASS(SpotLight, PointLight, "//meworla/spotlight");
		REPROPERTY(float, "cone_angle", Label("Cone Angle"), Edit, Read);
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
