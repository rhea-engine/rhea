/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GRAPHICS_CAMERA_H
#define RHEA_GRAPHICS_CAMERA_H

#include <Rhea/Event.h>
#include <Rhea/Core/Camera.h>
#include "GraphicsManager.h"

namespace poseidon
{
	class Core;
	class Camera;
	class Viewport;
}

namespace rhea::client {
	class Camera;
	
	class GraphicsCameraManager : public GraphicsManager
	{
	public:
		GraphicsCameraManager(GraphicsService* service) :
			GraphicsManager(service)
		{}

		// factory function
		bool Ensure(poseidon::Core* core, rhea::Unit* unit);

		// handle update event
		bool onUpdate(rhea::Event* ev, rhea::Unit* unit);

		// handle transform update
		void onTransformUpdate(rhea::Unit* unit) override;

		// handle frame update
		void onFrame(float delta) override;

		// handle events
		bool onItemDelete(rhea::Event* ev, rhea::Unit* unit);
	
		void syncCameraTransform(poseidon::Core* core, poseidon::CameraHandle handle, rhea::Transform* transform);
		void syncCameraProjection(poseidon::Core* core, poseidon::CameraHandle handle, rhea::Camera* sourceCamera, float viewportWidth, float viewportHeight);
	};
}
#endif // !RHEA_GRAPHICS_CAMERA_H
