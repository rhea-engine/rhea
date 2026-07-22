/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef POSEIDON_RENDERABLE_H
#define POSEIDON_RENDERABLE_H

#include <vector>

#include "RenderTypes.h"
#include "SkeletonPose.h"

namespace hippios
{
	class VertexBuffer;
	class IndexBuffer;
}

namespace poseidon
{
	class Mesh;
	class RenderQueue;
	class Camera;
	class Material;


	enum class RenderPhase : uint8_t
	{
		Background,
		Opaque,
		Transparent,
		Overlay
	};

	class Renderable
	{
	public:
		Renderable(Mesh* mesh) :
			mesh(mesh)
		{}
				
        RenderableHandle handle = InvalidRenderable;
		SkeletonPoseHandle skeleton_pose = InvalidSkeletonPoseHandle;
		bool skinned = false;

        Mesh* mesh = nullptr;
        Material* overrideMaterial = nullptr;

        math::Matrix4 worldTransform;
        math::Matrix4 inverseTranspose;

		bool scissorEnabled = false;
		ClipRect scissorRect{};

        bool visible = true;
        bool castsShadow = true;
        bool receivesShadow = true;

        uint32_t renderLayers = 0xFFFFFFFF;                
        RenderPhase renderPhase = RenderPhase::Opaque;
    };
}

#endif // !POSEIDON_RENDERABLE_H
