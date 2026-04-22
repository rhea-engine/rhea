#ifndef POSEIDON_RENDERWORLD_H
#define POSEIDON_RENDERWORLD_H

#include <cstdint>
#include <vector>
#include <unordered_map>

#include "RenderTypes.h"

namespace poseidon
{
	class Renderable;
	class Light;
	class Camera;
	class Core;
	class Mesh;
	class RenderContext; // keep old name during migration

	class RenderWorld
	{
	public:
		RenderWorld(Core* core) :
			m_core(core) 
		{ }
		~RenderWorld();

		RenderableHandle createRenderable(Mesh* mesh);
		void destroyRenderable(RenderableHandle handle);

		Camera* createCameraLegacy();
		CameraHandle createCamera();
		Camera* resolveCamera(CameraHandle handle);
		void destroyCamera(CameraHandle handle);

		LightHandle createLight();
		void destroyLight(LightHandle handle);

	
	private:
		Core* m_core; // for resource creation, maybe not needed if you pass renderer directly to the world

		struct RenderableRecord
		{
			poseidon::RenderableHandle id;
			poseidon::Renderable* legacyPtr = nullptr; // temporary only
			poseidon::Mesh* mesh = nullptr;
		};

		struct LightRecord
		{
			poseidon::LightHandle id;
			// no legacy node required if you do not need one
		};

		struct CameraRecord
		{
			poseidon::CameraHandle id;
			poseidon::Camera* legacyPtr = nullptr; // maybe direct camera object
		};

		CameraHandle m_nextCameraId = 0;
		RenderableHandle m_nextRenderableId = 0;
		LightHandle m_nextLightId = 0;

		std::unordered_map<RenderableHandle, RenderableRecord> m_renderables;
		std::unordered_map<CameraHandle, CameraRecord> m_cameras;
		std::unordered_map<LightHandle, LightRecord> m_lights;		
	};
}
#endif // !POSEIDON_RENDERWORLD_H
