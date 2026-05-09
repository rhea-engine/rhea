/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_RENDERCOMPONENT_H
#define RHEA_RENDERCOMPONENT_H

namespace poseidon
{
	class RenderView;
	class Mesh;
}

namespace rhea {
	class Mesh;
	class Material;
	class Skeleton;
}

#include <Poseidon/RenderTypes.h>
#include <Poseidon/RenderContext.h>
#include <Rhea/Component.h>
#include <Poseidon/SkeletonPose.h>

namespace rhea::client {

	/** GPU Reendering component, holds the mesh and material information of an entity
	*/
	class RenderComponent : public rhea::Component
	{
		RECLASS(RenderComponent, rhea::Component, "//meworla/rendercomponent")
	public:
		RenderComponent(std::string name, std::string type_name) :			
			renderable(poseidon::InvalidRenderable),
			light(poseidon::InvalidLight),
			camera(poseidon::InvalidCamera),
			ctx(nullptr),
			gpuMesh(nullptr),
			skeleton(nullptr),
			rhea::Component(name, type_name)
		{}
		RenderComponent(std::string name, poseidon::GraphicsNodeId id) :			
			RenderComponent(name, "//meworla/rendercomponent")
		{ gfx_node_id = id; }
	
		poseidon::RenderContext* ctx = nullptr;  // non-owning (Poseidon owns)

		// Poseidon registrations (non-owning)
		poseidon::RenderableHandle renderable;
		poseidon::LightHandle light;
		poseidon::CameraHandle camera;		

		// cached GPU resources (non-owning, owned by caches / source systems)
		poseidon::Mesh* gpuMesh = nullptr;
		rhea::Skeleton* skeleton = nullptr;
		poseidon::SkeletonPoseHandle skeleton_pose = poseidon::InvalidSkeletonPoseHandle;

		bool hasRenderable() const { return renderable != poseidon::InvalidRenderable; }
		bool hasLight() const { return light != poseidon::InvalidLight; }
		bool hasCamera() const { return camera != poseidon::InvalidCamera; }

		void clearRegistrations()
		{
			renderable = poseidon::InvalidRenderable;
			light = poseidon::InvalidLight;
			camera = poseidon::InvalidCamera;
			ctx = nullptr;
		}

		poseidon::GraphicsNodeId gfx_node_id = poseidon::InvalidGraphicsNode;
	};
}

#endif // !RHEA_RENDERCOMPONENT_H
