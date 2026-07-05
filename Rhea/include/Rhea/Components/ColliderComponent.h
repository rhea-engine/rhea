/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_COLLIDERCOMPONENT_H
#define RHEA_COLLIDERCOMPONENT_H

#include <Rhea/Unit.h>
#include <Math/Vector3.h>
#include <Rhea/Ref.h>

namespace rhea
{
	class Mesh;

	class ColliderShape {
	public:
		virtual ~ColliderShape() = default;
		virtual std::string getShapeType() const { return "None"; }
		virtual ColliderShape* clone() const { return new ColliderShape(); }
	};

	class BoxColliderShape : public ColliderShape {
	public:
		math::Vector3 half_extents;
		std::string getShapeType() const override { return "Box"; }
		BoxColliderShape* clone() const { return new BoxColliderShape(*this); }
	};

	class SphereColliderShape : public ColliderShape {
	public:
		float radius;
		std::string getShapeType() const override { return "Sphere"; }
		SphereColliderShape* clone() const { return new SphereColliderShape(*this); }
	};

	class CapsuleColliderShape : public ColliderShape {
	public:
		float radius;
		float height;
		std::string getShapeType() const override { return "Capsule"; }
		CapsuleColliderShape* clone() const { return new CapsuleColliderShape(*this); }
	};

	class MeshColliderShape : public ColliderShape {
	public:
		enum class MeshColliderMode {
			Convex,
			TriangleMesh
		};
		MeshColliderMode mode = MeshColliderMode::Convex;
		Ref<Mesh> mesh;
		std::string getShapeType() const override { return "Mesh"; }
		MeshColliderShape* clone() const { return new MeshColliderShape(*this); }
	};

	class ColliderComponent : public Component
	{
		RECLASS(ColliderComponent, Component, "//meworla/collidercomponent")
	public:
		ColliderComponent(std::string name, std::string type_name) :
			Component(name, type_name)
		{
		}

		// Clone method
		ColliderComponent* Clone() const override { return new ColliderComponent(*this); }

		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values) override;

		bool enabled = true;
		ColliderShape* shape = nullptr;
		math::Vector3 offset;

	protected:
		// Copy constructor
		ColliderComponent(const ColliderComponent& other) :
			rhea::Component(other),
			enabled(other.enabled),
			offset(other.offset)
		{
			shape = other.shape ? other.shape->clone() : nullptr;
		}
	};
}
#endif // !RHEA_COLLIDERCOMPONENT_H
