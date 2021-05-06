#include "GUIController.h"
#include "Application.h"
#include "UnitTree.h"
#include "Scene.h"

#include "GuiService.h"

#include "GUIController.h"
#include "Widget.h"
#include "Label.h"
#include <Rhea/SceneLoader.h>

namespace Rhea
{
	void GUIController::setup(Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
	{
		Unit::setup(app, attributes, values);
		m_status = Status::SETUP;
	}
	
	void GUIController::ready()
	{
		if (m_status >= Status::READY)
			return;
		m_status = Status::READY;

		// prepare all children
		Unit::ready();

		// all children are ready - initial resize
		for (auto it = m_children.begin(); it != m_children.end(); ++it)
		{
			Widget* w = (Widget*)it->second;
			if (w->getValue("isWidget", false)) {
				w->setToplevel(true);
				w->onParentSized(nullptr);
				w->onParentMoved();
			}
		}
	}

	void GUIController::load(Orion::ResourceRequest* req, ReadyHandler handler)
	{
		// for now we assume a scene file
		std::unique_ptr<SceneLoader> parser = std::make_unique<SceneLoader>();

		// parse the file
		parser->load((Scene*)this, req->getResource(), m_app);

		// assume everything is fine
		handler(ReqError::OK);
	}
	
	Unit* GUIController::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new GUIController(name, type);
		return ret;
	}

	void GUIController::Register(Application* app)
	{
		SimpleParser::Type::registerType<GUIController>("//meworla/gui");

		// register widget types
		SimpleParser::Type::registerType<Widget>("//meworla/gui/widget");
		SimpleParser::Type::registerType<Label>("//meworla/gui/label");

		SimpleParser::Type::registerType<GuiService>("//meworla/gui/service"); 
		
		GuiService* service = (GuiService*)SimpleParser::Type::create("GuiService", "//meworla/gui/service");
		service->setup(app);
		app->registerService("gui", service);
	}
}