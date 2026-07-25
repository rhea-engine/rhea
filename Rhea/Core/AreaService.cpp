/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include <Rhea/Core/AreaService.h>

namespace rhea
{
	void AreaService::unregisterArea(Area* area)
	{
		m_areas.erase(area);
		for (auto it = m_inside.begin();
			it != m_inside.end();)
		{
			if (it->first == area)
				it = m_inside.erase(it);
			else
				++it;
		}
	}	
	
	void AreaService::registerTarget(Item* item)
	{
		if (!item)
			return;

		if (m_targets.count(item) > 0) {
			return;
		}
		m_targets.insert(item);
	}

	void AreaService::unregisterTarget(Item* item)
	{
		if (!item)
			return;

		m_targets.erase(item);

		for (auto it = m_inside.begin(); it != m_inside.end();)
		{
			if (it->second == item)
				it = m_inside.erase(it);
			else
				++it;
		}
	}

	bool AreaService::onFrame(float delta)
	{
		std::vector<Area*> areas(
			m_areas.begin(),
			m_areas.end()
		);

		std::vector<Item*> targets(
			m_targets.begin(),
			m_targets.end()
		);

		// iterate over areas
		for (auto& area : areas)
		{
			// check for unregistered areas by callback
			if (!area || !m_areas.count(area))
				continue;

			auto* collider = doCast<ColliderComponent>(area->getCollider());
			BoxColliderShape* box_shape = nullptr;
			if (collider)
				box_shape = static_cast<BoxColliderShape*>(collider->shape);

			if (!box_shape)
				continue;

			Vector3 center = area->getTransform()->getWorldPosition().getTranslation() + collider->offset;

			// iterate over targets
			for (auto& target : targets) {

				// check for unregistered targets by callback
				if (!target || !m_targets.count(target))
					continue;

				// check for collisions with other items
				
				auto* transform = target->getTransform();
				if (!transform)
					continue;
				Vector3 target_position = transform->getWorldPosition().getTranslation();
				Vector3 delta_pos = target_position - center;

				bool inside =
					std::abs(delta_pos.x) <= box_shape->half_extents.x &&
					std::abs(delta_pos.y) <= box_shape->half_extents.y &&
					std::abs(delta_pos.z) <= box_shape->half_extents.z;

				// get old state
				auto key = std::make_pair(area, target);

				if (inside) {
					auto [it, inserted] = m_inside.insert(key);

					if (inserted) {
						// target has entered the area
						area->onTargetEnter(target);						
					}
					else {
						area->onTargetStay(target, delta);
					}
				}
				else {
					if (m_inside.erase(key) > 0)
						// target has exited the area
						area->onTargetExit(target);
				}
			}
		}

		return false;
	}
}