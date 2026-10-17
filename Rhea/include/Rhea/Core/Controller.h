/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_CONTROLLER_H
#define RHEA_CONTROLLER_H

#include <Rhea/Core/Item.h>
#include <Rhea/Core/ActiveUnit.h>
#include <Psio/Input/InputEvent.h>
#include <Rhea/Core/SceneResource.h>
#include <memory>

namespace rhea
{
	// class to controll an object with user input
	class Controller : public ActiveUnit
	{
		MECLASS_VIRTUAL(Controller, ActiveUnit)
	public:
		Controller(std::string path_name, std::string type_name) :
			m_target(NULL),
			ActiveUnit(path_name, type_name)
		{}
		void setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values) override;

		// handle input event (after Input Map was applied)
		virtual bool onInput(Event* ev, Unit* unit) = 0;

		// set the controlled unit
		virtual void connect(Item* target) { m_target = target; }
		virtual void disconnect(bool reset_input = true) { m_target = nullptr; }
		virtual void switchTarget(Item* target, bool reset_input) { m_target = target; }

		void onFrame(float delta) override {}

		// get the target
		Item* getTarget() { return m_target; }

		// get and set the default character for this controller
		Ref<SceneResource> getDefaultCharacter() const { return m_default_character; }
		void setDefaultCharacter(const Ref<SceneResource>& resource) { m_default_character = resource; }

		// type initialization
		static void initializeType(simpleparser::Type* type);

	protected:
		Item* m_target;
		Ref<SceneResource> m_default_character;
	};
}
#endif // !RHEA_CONTROLLER_H
