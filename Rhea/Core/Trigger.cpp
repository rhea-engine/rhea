/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#include <Rhea/Core/Trigger.h>
#include <Psio/Log.h>

namespace rhea
{
	void Trigger::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Area::setup(app, attributes, values);
	}

	void Trigger::ready()
	{
		Area::ready();
	}

	void Trigger::onTargetEnter(Item* item)
	{
		if (!item)
			return;

		Event e("area_enter", this);
		item->sendEvent("area_enter", &e);
	}
	
	void Trigger::onTargetExit(Item* item)
	{
		if (!item)
			return;

		Event e("area_exit", this);
		item->sendEvent("area_exit", &e);
	}
}