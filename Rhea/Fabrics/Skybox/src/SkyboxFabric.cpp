/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Skybox/SkyboxFabric.h"
#include <Rhea/Skybox/Skybox.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <Psio/Log.h>

namespace rhea
{
	// register skybox
	void SkyboxFabric::Register(Application* app)
	{
        Skybox::_register_Skybox();

        // PhysicsManagerService existiert bereits - wurde in Application::init() erstellt
        auto* physics_mgr = static_cast<PhysicsManagerService*>(app->getService("physics_mgr"));
        if (!physics_mgr) {
            LOG(LOG_ERROR) << "No physics manager found.";
            return;
        }
        // Opt-out: Skybox braucht keine Physics
        PhysicsBodyDesc none;
        none.kind = "";
        physics_mgr->registerBodyForType(Skybox::staticType(), none);

	}
}