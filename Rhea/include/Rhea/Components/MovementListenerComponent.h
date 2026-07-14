/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_MOVEMENTLISTENERCOMPONENT_H
#define RHEA_MOVEMENTLISTENERCOMPONENT_H

#include <Rhea/Component.h>
#include <Math/Vector3.h>

namespace rhea
{
	class MovementListenerComponent : public Component
	{
		RECLASS_VIRTUAL(MovementListenerComponent, Component, "//meworla/movementlistener")
	public:
		MovementListenerComponent(std::string name, std::string type_name) :
			Component(name, type_name)
		{
		}
		virtual void onMovementChanged(const math::Vector3& move) = 0;
		virtual void onTurnChanged(float turn) = 0;
		virtual void onTransformChanged() = 0;
	};
}

#endif // !RHEA_MOVEMENTLISTENERCOMPONENT_H
