/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Poseidon/FrameRenderer.h>
#include <Poseidon/Material.h>
#include <Poseidon/Shader.h>
#include <Math/Matrix3x4.h>
#include <Math/Matrix4.h>

#include "SceneManager.h"
#include "ItemManager.h"
#include <RheaClient/Graphics/CameraManager.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Item2D.h>
#include <RheaClient/Graphics/GraphicsService.h>
#include <Psio/Log.h>
#include <Rhea/Core/Lighting.h>

namespace rhea::client {
	const std::vector<rhea::Item* >& SceneRenderState::getItems()
	{
		if (!m_items_valid) {
			m_items = m_scene->getItems();
			m_items_valid = true;
		}
		return m_items;
	}	

	bool GraphicsSceneManager::Ensure(poseidon::Core* core, rhea::Unit* unit)
	{
		rhea::Scene* scene = doCast<rhea::Scene>(unit);
		if (!scene) {
			LOG(LOG_ERROR) << "GraphicsSceneManager::Ensure: unit is not a scene ";
			return false;
		}

		m_core = core;
		auto* state = scene->getComponent<SceneRenderState>();
		const bool first_initialization = state == nullptr;

		if (first_initialization) {
			// The state marks only the one-time event setup. Rendering resources
			// still have to be ensured whenever this scene becomes active again.
			scene->addComponentUnique<SceneRenderState>(scene);
		}

		// Re-run this for every activation. Individual managers reuse existing
		// resources and restore scene-dependent state such as the active camera.
		scene->traverse([&](std::string& name, rhea::Component* c)->bool {
			rhea::Unit* u = doCast<rhea::Unit>(c);
			if (u && c != scene) m_service->addElement(u);
			return true;
			});
		return true;
	}
}
