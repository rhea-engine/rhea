/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/GUI/GUIFabric.h>
#include <Rhea/GUI/Widget.h>
#include <Rhea/GUI/Label.h>
#include <Rhea/GUI/Button.h>
#include <Rhea/GUI/Dialog.h>
#include <Rhea/GUI/List.h>
#include <Rhea/GUI/ListItem.h>
#include <Rhea/GUI/Styles/StyleSheet.h>
#include <Rhea/GUI/CssLoader.h>
#include <Rhea/GUI/GuiService.h>
#include <Rhea/GUI/GUIController.h>
#include <Rhea/GUI/Box.h>
#include <Rhea/GUI/Column.h>
#include <Rhea/GUI/Row.h>
#include <Rhea/GUI/TextInput.h>
#include <Rhea/Physics/PhysicsManagerService.h>
#include <Rhea/Text/TextWidget.h>
#include <Rhea/GUI/Spacer.h>
#include <Rhea/GUI/ScrollView.h>

namespace rhea
{
	void rhea::GUIFabric::Register(rhea::Application* app)
	{
		// register widget types
		simpleparser::Type::registerType<Widget>("//meworla/gui/widget");

		// layout types
		simpleparser::Type::registerType<BoxWidget>("//meworla/gui/box");
		ColumnWidget::_register_ColumnWidget();
		simpleparser::Type::registerType<RowWidget>("//meworla/gui/row");
		simpleparser::Type::registerType<SpacerWidget>("//meworla/gui/spacer");
		ScrollView::_register_ScrollView();

		// basic widgets
		simpleparser::Type::registerType<Label>("//meworla/gui/label");
		simpleparser::Type::registerType<TextInput>("//meworla/gui/textinput");
		simpleparser::Type::registerType<Button>("//meworla/gui/button");
		simpleparser::Type::registerType<Dialog>("//meworla/gui/dialog");
		simpleparser::Type::registerType<ListWidget>("//meworla/gui/list");
		simpleparser::Type::registerType<ListWidgetItem>("//meworla/gui/listitem");

		// general gui types
		simpleparser::Type::registerType<GUIController>("//meworla/gui");

		// register helpers
		app->getResourceLoader()->registerLoader<CssLoader>("text/css");
		simpleparser::Type::registerType<GuiService>("//meworla/gui/service");

		GuiService* service = simpleparser::Type::create<GuiService>("GuiService");
		service->setup(app);
		app->registerService("gui", service);

		// no physics
		auto* mgr = static_cast<PhysicsManagerService*>(app->getService("physics_mgr"));
		if (mgr) {
			PhysicsBodyDesc none;
			none.kind = ""; // means disabled
			mgr->registerBodyForType(TextWidget::staticType(), none);
		}
	}
}
