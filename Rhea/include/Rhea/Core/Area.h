/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */
#ifndef RHEA_AREA_H
#define RHEA_AREA_H

#include <Rhea/Core/Item.h>
#include <Rhea/Components/ColliderComponent.h>

namespace rhea
{
	class Area : public Item
	{
		RECLASS(Area, Item, "//meworla/area");
	public:
		Area(std::string name, std::string type_name) :
			Item(name, type_name)
		{}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

		void ready();

		ColliderComponent* getCollider() const { return m_collider; }
		// TODO uint32_t getTargetMask() const;
		// TODO int getPriority() const;

		virtual void onTargetEnter(Item* target) {}
		virtual void onTargetStay(Item* target, float delta) {}
		virtual void onTargetExit(Item* target) {}

		void onActivate() override;
		void onDeactivate() override;

	protected:
		ColliderComponent* m_collider = nullptr;
	};
}

#endif // !RHEA_AREA_H