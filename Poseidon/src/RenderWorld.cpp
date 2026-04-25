#include "Poseidon/RenderWorld.h"
#include <Poseidon/Mesh.h>
#include <Poseidon/Core.h>
#include <Poseidon/Entity.h>
#include <Poseidon/Camera.h>
#include <Poseidon/Renderable.h>
#include <Poseidon/Light.h>

namespace poseidon
{
	RenderWorld::~RenderWorld()
	{
	}

	RenderableHandle RenderWorld::createRenderable(Mesh* mesh)
	{
		auto* legacy = static_cast<Renderable*>(m_core->createEntity(mesh)); // old path, temporary
		RenderableHandle h = ++m_nextRenderableId;
		m_renderables[h] = { h, legacy };
		return h;
	}

	Renderable* RenderWorld::resolveRenderable(RenderableHandle handle)
	{
		auto it = m_renderables.find(handle);
		if (it != m_renderables.end()) {		
			return it->second.ptr;
		}
		else
			return nullptr;
	}

	void RenderWorld::destroyRenderable(RenderableHandle handle)
	{
		auto it = m_renderables.find(handle);
		if (it != m_renderables.end()) {
			delete it->second.ptr;
			m_renderables.erase(it);
		}
	}

	CameraHandle RenderWorld::createCamera()
	{
		auto* cam = new Camera(m_core->get_renderer());
		CameraHandle h = ++m_nextCameraId;
		m_cameras[h] = { h, cam };
		return h;
	}

	LightHandle RenderWorld::createLight()
	{
		LightHandle h = ++m_nextLightId;
		Light* light = new Light();
		m_lights[h] = { h, light };
		return h;
	}

	void RenderWorld::destroyLight(LightHandle handle)
	{
		auto it = m_lights.find(handle);
		if (it != m_lights.end()) {
			delete it->second.ptr;
			m_lights.erase(it);
		}
	}

	Light* RenderWorld::resolveLight(LightHandle handle)
	{
		auto it = m_lights.find(handle);
		if (it != m_lights.end()) {		
			return it->second.ptr;
		}
		else
			return nullptr;
	}

	Camera* RenderWorld::resolveCamera(CameraHandle handle)
	{
		auto it = m_cameras.find(handle);
		if (it != m_cameras.end())
			return it->second.ptr;
		else
			return nullptr;
	}

	void RenderWorld::destroyCamera(CameraHandle handle)
	{
		auto it = m_cameras.find(handle);
		if (it != m_cameras.end()) {
			delete it->second.ptr;
			m_cameras.erase(it);
		}
	}

	SkeletonPoseHandle RenderWorld::createSkeletonPose(size_t boneCount)
	{
		SkeletonPoseHandle h = m_nextSkeletonPose++;

		SkeletonPose pose;
		pose.setBoneCount(boneCount);

		m_skeletons.emplace(h, std::move(pose));
		return h;
	}

	SkeletonPose* RenderWorld::getSkeletonPose(SkeletonPoseHandle handle)
	{
		auto it = m_skeletons.find(handle);
		if (it == m_skeletons.end())
			return nullptr;

		return &it->second;
	}
	void RenderWorld::destroySkeletonPose(SkeletonPoseHandle handle)
	{
		m_skeletons.erase(handle);
	}
}