/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_AREA_SERVICE_H
#define RHEA_AREA_SERVICE_H

#include <Rhea/Service.h>
#include <Rhea/Core/Item.h>
#include <Rhea/Core/Area.h>

#include <functional>
#include <utility>
#include <unordered_set>

namespace rhea
{
	class AreaService : public Service
	{
		RECLASS(AreaService, Service, "//meworla/areaservice")
	public:
		AreaService(std::string name, std::string type_name) :
			Service(std::move(name), std::move(type_name))
		{}
		void registerArea(Area* area) { m_areas.insert(area); }
		void unregisterArea(Area* area);

		void registerTarget(Item* item);
		void unregisterTarget(Item* item);

		bool onFrame(float delta) override;

	protected:
		std::unordered_set<Area*> m_areas;
		std::unordered_set<Item*> m_targets;

		struct AreaTargetHash
		{
			std::size_t operator()(
				const std::pair<Area*, Item*>& value
				) const noexcept
			{
				std::size_t areaHash =
					std::hash<Area*>{}(value.first);

				std::size_t itemHash =
					std::hash<Item*>{}(value.second);

				return areaHash ^
					(itemHash + 0x9e3779b9 +
						(areaHash << 6) +
						(areaHash >> 2));
			}
		};
		std::unordered_set<std::pair<Area*, Item*>,	AreaTargetHash> m_inside;
	};
}

#endif // !RHEA_AREA_SERVICE_H