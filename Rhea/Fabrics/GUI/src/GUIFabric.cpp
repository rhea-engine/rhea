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
#include <Rhea/Fabrics/Text/TextWidget.h>

namespace Rhea
{
	void Rhea::GUIFabric::Register(Rhea::Application* app)
	{
		// register widget types
		SimpleParser::Type::registerType<Widget>("//meworla/gui/widget");

		// layout types
		SimpleParser::Type::registerType<BoxWidget>("//meworla/gui/box");
		SimpleParser::Type::registerType<ColumnWidget>("//meworla/gui/column");
		SimpleParser::Type::registerType<RowWidget>("//meworla/gui/row");

		// basic widgets
		SimpleParser::Type::registerType<Label>("//meworla/gui/label");
		SimpleParser::Type::registerType<TextInput>("//meworla/gui/textinput");
		SimpleParser::Type::registerType<Button>("//meworla/gui/button");
		SimpleParser::Type::registerType<Dialog>("//meworla/gui/dialog");
		SimpleParser::Type::registerType<ListWidget>("//meworla/gui/list");
		SimpleParser::Type::registerType<ListWidgetItem>("//meworla/gui/listitem");

		// general gui types
		SimpleParser::Type::registerType<GUIController>("//meworla/gui");

		// register helpers
		app->getResourceLoader()->registerLoader<CssLoader>("text/css");
		SimpleParser::Type::registerType<GuiService>("//meworla/gui/service");

		GuiService* service = SimpleParser::Type::create<GuiService>("GuiService");
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
