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

	class Renderable
	{
	public:
		Renderable(Mesh* mesh) :
			mesh(mesh)
		{}
				
        RenderableHandle handle = InvalidRenderable;
		SkeletonPoseHandle skeleton_pose = InvalidSkeletonPoseHandle;

        Mesh* mesh = nullptr;
        Material* overrideMaterial = nullptr;

        math::Matrix4 worldTransform;
        math::Matrix4 inverseTranspose;

        bool visible = true;
        bool castsShadow = true;
        bool receivesShadow = true;

        uint32_t renderLayers = 0xFFFFFFFF;                
    };
}

#endif // !POSEIDON_RENDERABLE_H
