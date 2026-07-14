/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEACLIENT_TRANSFORM_LISTENER_H
#define RHEACLIENT_TRANSFORM_LISTENER_H

#include <Rhea/Components/MovementListenerComponent.h>
#include <RheaClient/Graphics/GraphicsManager.h>

namespace rhea::client {
	class TransformListener : public rhea::MovementListenerComponent
	{
		RECLASS(TransformListener, rhea::MovementListenerComponent, "//meworla/transformlistener")
	public:
		TransformListener(std::string name, std::string type_name) : MovementListenerComponent(name, type_name) {}
		~TransformListener() = default;

		void setGraphicsManager(GraphicsManager* mgr) { m_mgr = mgr; }

		void onMovementChanged(const math::Vector3& move) override {}
		void onTurnChanged(float turn) override {}
		void onTransformChanged() override { if (m_mgr) m_mgr->onTransformUpdate(m_parent); }

	private:
		GraphicsManager* m_mgr = nullptr;
	};
}
#endif // !RHEACLIENT_TRANSFORM_LISTENER_H