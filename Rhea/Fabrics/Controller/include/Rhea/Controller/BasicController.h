/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_BASICCONTROLLER_H
#define RHEA_BASICCONTROLLER_H

#include <Rhea/Core/Controller.h>
#include <Math/Vector3.h>
#include <Math/Quaternion.h>
#include <Rhea/Core/Position.h>

namespace rhea
{
	class BasicController : public Controller
	{
		MECLASS(BasicController, Controller)
	public:
		BasicController(std::string path_name, std::string type_name) :			
			Controller(path_name, type_name)
		{}

		void ready() override;

		void onActivate() override;
		void onDeactivate() override;

		// set the position manually
		void setLocalPosition(const Position& position);
		void setWorldPosition(const Position& position);
		void lookAt(math::Vector3& target);

		math::Vector3 getWorldPosition();

		// Controller functions

		// input event handler
		bool onInput(Event* ev, Unit* unit);

		void onFrame(float delta) override {};

		void connect(Item* target);

		void logMe();

		// get the current orientation
		math::Quaternion getOrientation() {	if (m_target) return m_target->getTransform()->getWorldPosition().getRotation(); else return math::Quaternion(); }

	protected:
		math::Vector3 m_move;
		float m_turn = 0;
		math::Vector3 m_last_move;
		float m_last_turn = 0;
	};
}

#endif // !RHEA_BASICCONTROLLER_H
