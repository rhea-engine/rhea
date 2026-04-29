/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_AVATAR_H
#define RHEA_AVATAR_H

#include <Rhea/Unit.h>
#include <Math/Vector3.h>

namespace rhea
{
	class Avatar : public Unit
	{
		MECLASS(Avatar, Unit)
	public:
		Avatar(std::string name, std::string type_name) :
			Unit(name, type_name)
		{}
		Avatar(std::string name) :
			Avatar(name, "//meworla/avatar")
		{}

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		math::Vector3 getPosition() { return m_position; }
		void setPosition(math::Vector3 position) { m_position = position; }

		void setModel(std::string model) { m_modelname = model; m_values["model"] = model; }
		std::string getModel() { return m_modelname; }

	protected:
		math::Vector3 m_position;
		std::string m_modelname;
	};
}

#endif // !RHEA_AVATAR_H
