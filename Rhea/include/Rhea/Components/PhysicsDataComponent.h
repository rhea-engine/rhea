/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_PHYSICSDATACOMPONENT_H
#define RHEA_PHYSICSDATACOMPONENT_H

#include <vector>

#include <Rhea/Unit.h>
#include <Math/Vector3.h>
#include <Rhea/Core/Mesh.h>
#include "ColliderComponent.h"

namespace rhea
{
	struct RigidBody
	{
		RESTRUCT(RigidBody, "//meworla/rigidbody");

		enum class MotionType {
			Static,
			Dynamic,
			Kinematic
		};

		REENUM(MotionType, "//meworla/rigidbody/motiontype", Static, Dynamic, Kinematic);

		REPROPERTY(MotionType, "motion", Label("Motion Type"), Edit, Read);
		REPROPERTY(float, "mass", Label("Mass"), Edit, Read);
		REPROPERTY(float, "gravity", Label("Gravity"), Edit, Read);
		REPROPERTY(bool, "ground_check", Label("Ground Check"), Edit, Read);
		REPROPERTY(float, "ground_height", Label("Ground Height"), Edit, Read);
		REPROPERTY(float, "friction", Label("Friction"), Edit, Read);
		REPROPERTY(math::Vector3, "velocity", Label("Velocity"), Edit, Read);

		MotionType motion = MotionType::Static;

		std::string kind = "RheaPhysics:RigidBody";
		float mass = 0.0f;
		float gravity = 9.81f;
		bool ground_check = false;
		float ground_height = 0.0f;
		float friction = 0.5f;
		math::Vector3 velocity;
	};

	class PhysicsDataComponent : public Unit
	{
		RECLASS(PhysicsDataComponent, Unit, "//meworla/physicsdatacomponent");
		REPROPERTY(RigidBody, "rigid_body", Label("Rigid Body"), Edit, Read);	
	public:
		PhysicsDataComponent(std::string name, std::string type_name) :
			Unit(name, type_name)
		{}

		PhysicsDataComponent* Clone() const override { return new PhysicsDataComponent(*this); }

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		void ready() override;
		
		bool enabled = true;

		RigidBody rigid_body;
		std::vector<ColliderComponent*> colliders;

	protected:
		// copy constructor
		PhysicsDataComponent(const PhysicsDataComponent& other) :
			Unit(other.m_pathname, other.getTypeName()),
			enabled(other.enabled),
			rigid_body(other.rigid_body)
		{
			// Deep copy der colliders
			for (auto* collider : other.colliders) {
				if (collider) {
					colliders.push_back(static_cast<ColliderComponent*>(collider->Clone()));
				}
			}
		}
	};
}
#endif // !RHEA_PHYSICSDATACOMPONENT_H
