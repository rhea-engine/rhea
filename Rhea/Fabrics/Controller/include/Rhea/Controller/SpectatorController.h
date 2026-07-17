/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_SPECTATORCONTROLLER_H
#define RHEA_SPECTATORCONTROLLER_H

#include "BasicController.h"
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <Rhea/Core/Position.h>

namespace rhea
{
	class Camera;

	class SpectatorController : public BasicController
	{
		MECLASS(SpectatorController, BasicController)
	public:
		SpectatorController(std::string path_name, std::string type_name) :
			m_view(NULL),
			BasicController(path_name, type_name)
		{}

		void ready() override;

		void onActivate() override;
		void onDeactivate() override;

		// set the position manually
		void setPosition(math::Vector3 position);
		void lookAt(math::Vector3& target);

		// Controller functions

		void onFrame(float delta) override;

		// input event handler
		bool onInput(Event* ev, Unit* unit);

	protected:
		Camera* m_view;

		// Offset in target-oriented space.
		// Affected by target rotation, but intentionally not by target scale.
		math::Vector3 m_follow_offset;

		void updateCamera(float delta);

		// should the camera follow the rotation of the target?
		bool m_follow_rotation = true;

		// m_camera_slide < 0  // slide left
		// m_camera_slide > 0  // slide right
		float m_camera_slide = 0.0f;

	};
}

#endif // !RHEA_SPECTATORCONTROLLER_H
