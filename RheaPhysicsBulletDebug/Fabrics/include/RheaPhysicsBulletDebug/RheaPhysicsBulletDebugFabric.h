/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_RHEAPHYSICSFABRICBULLETDEBUG_H
#define RHEA_RHEAPHYSICSFABRICBULLETDEBUG_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea::physics::bullet::debug
{
	class RheaPhysicsBulletDebugFabric : public rhea::IFabric
	{
	public:
		void Register(rhea::Application* app) override;

		const char* getName() const override { return "RheaPhysicsBulletDebug"; }
	};
}

#endif // !RHEA_RHEAPHYSICSBULLETDEBUG_H
