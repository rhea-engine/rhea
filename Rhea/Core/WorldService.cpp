/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Core/WorldService.h>

#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/ActiveUnit.h>

namespace rhea
{
	void WorldService::onInit()
	{
		m_app->getUnitTree()->registerEventHandler("scene_activate", this, [&](Event* ev, Unit* tree)->bool {
			onSceneActivate(doCast<Scene>(ev->getTarget()));
			return false;
			});
		m_app->getUnitTree()->registerEventHandler("scene_deactivate", this, [&](Event* ev, Unit* tree)->bool {
			onSceneDeactivate(doCast<Scene>(ev->getTarget()));
			return false;
			});
	}

	bool WorldService::onFrame(float delta)
	{
		// update items
		for (auto it = m_scenes.begin(); it < m_scenes.end(); ++it) {
			(*it)->traverse([delta](std::string& name, Component* unit)->bool {
				ActiveUnit* active_unit = doCast<ActiveUnit>(unit);
				if (active_unit) {
					active_unit->onFrame(delta);
				}
				Item* item = doCast<Item>(unit);
				if (item) {					
					// TODO: Check if this is necessary
					if (item->getTransform())
						item->getTransform()->update();
				}
				return true;
				});
		}
		return false;
	}

	bool WorldService::onFrameFinal(float delta)
	{
		return false;
	}

	void WorldService::onSceneActivate(Scene* scene)
	{
		m_scenes.push_back(scene);
	}

	void WorldService::onSceneDeactivate(Scene* scene)
	{
		m_scenes.erase(std::remove(m_scenes.begin(), m_scenes.end(), scene), m_scenes.end());
	}
}