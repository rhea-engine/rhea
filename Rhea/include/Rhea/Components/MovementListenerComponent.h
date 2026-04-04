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
		virtual void onMovementChanged(const Math::Vector3& move) = 0;
		virtual void onTurnChanged(float turn) = 0;
		virtual void onPositionChanged() = 0;
	};
}

#endif // !RHEA_MOVEMENTLISTENERCOMPONENT_H
