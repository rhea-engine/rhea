/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Components/ColliderComponent.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Mesh.h>

namespace rhea
{
	void ColliderComponent::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);

		if (values.find("enabled") != values.end()) {
			enabled = values["enabled"].toBool();
		}
		if (values.find("offset") != values.end()) {
			std::vector<Variant> offset_arr = values["offset"].toArray();
			if (offset_arr.size() == 3) {
				offset = math::Vector3(offset_arr[0].toFloat(), offset_arr[1].toFloat(), offset_arr[2].toFloat());
			}
		}
		if (values.find("shape") != values.end()) {
			std::string shape_str = values["shape"].toString();
			if (shape_str == "box") {
				BoxColliderShape* box_shape = new BoxColliderShape();
				if (values.find("half_extents") != values.end()) {
					std::vector<Variant> half_extents_arr = values["half_extents"].toArray();
					if (half_extents_arr.size() == 3) {
						box_shape->half_extents = math::Vector3(half_extents_arr[0].toFloat(), half_extents_arr[1].toFloat(), half_extents_arr[2].toFloat());
					}
				}
				shape = box_shape;
			}
			else if (shape_str == "sphere") {
				SphereColliderShape* sphere_shape = new SphereColliderShape();
				if (values.find("radius") != values.end()) {
					sphere_shape->radius = values["radius"].toFloat();
				}
				shape = sphere_shape;
			}
			else if (shape_str == "capsule") {
				CapsuleColliderShape* capsule_shape = new CapsuleColliderShape();
				if (values.find("radius") != values.end()) {
					capsule_shape->radius = values["radius"].toFloat();
				}
				if (values.find("height") != values.end()) {
					capsule_shape->height = values["height"].toFloat();
				}
				shape = capsule_shape;
			}
			else if (shape_str == "mesh") {
				MeshColliderShape* mesh_shape = new MeshColliderShape();
				if (values.find("mode") != values.end()) {
					std::string mode_str = values["mode"].toString();
					if (mode_str == "convex") {
						mesh_shape->mode = MeshColliderShape::MeshColliderMode::Convex;
					}
					else if (mode_str == "triangle_mesh") {
						mesh_shape->mode = MeshColliderShape::MeshColliderMode::TriangleMesh;
					}
				}
				if (values.find("mesh") != values.end()) {
					std::string mesh_path = values["mesh"].toString();
					mesh_shape->mesh.setId(mesh_path);
				}
				shape = mesh_shape;
			}
		}
	}
}