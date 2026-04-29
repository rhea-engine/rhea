/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Application.h>

#include <Rhea/Animation/AnimationFabric.h>
#include <Rhea/Animation/Animator.h>
#include <Rhea/Animation/Animation.h>
#include <Rhea/Physics/PhysicsManagerService.h>

namespace rhea
{
	void AnimationFabric::Register(Application* app)
	{
		simpleparser::Type::registerType<Animator>("//meworla/animator");
		simpleparser::Type::registerType<Animation>("//meworla/animation");
		simpleparser::Type::registerType<Skeleton>("//meworla/skeleton");

		// no physics
		auto* mgr = static_cast<PhysicsManagerService*>(app->getService("physics_mgr"));
		if (mgr) {
			PhysicsBodyDesc none;
			none.kind = ""; // means disabled
			mgr->registerBodyForType(Animator::staticType(), none);
			mgr->registerBodyForType(Animation::staticType(), none);
			mgr->registerBodyForType(Skeleton::staticType(), none);
		}
	}
}