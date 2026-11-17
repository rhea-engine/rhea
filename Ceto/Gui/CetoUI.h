#pragma once

#include <Rhea/Core/Item.h>
#include <vector>
#include <string>
#include <Rhea/Component.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/SceneResource.h>
#include <Rhea/Ref.h>
#include <unordered_map>
#include <SimpleParser/Variant.h>
#include <Rhea/Application.h>
#include <Rhea/GUI/GUIController.h>
#include "OpenBrowser.h"
#include <Rhea/Error.h>
#include "AlertDialog.h"
#include <Rhea/GUI/Dialog.h>
#include "BrowserPlace.h"
#include <functional>

class BrowserController;

class CetoUI : public rhea::Item
{
	RECLASS(CetoUI, rhea::Item, "//ceto/uicontroller")
public:
	using OpenHandler = std::function<void(const std::string&)>;

	CetoUI(std::string name, std::string type_name) :
		rhea::Item(name, type_name)
	{}
	void setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values) override;
	void ready() override;
	void initializeBrowser(BrowserController* controller);

	std::vector<rhea::Unit::ResourceReference> getReferences();

	void setBrowserData(const BrowserData& data);
	void setRecentLocations(const std::vector<std::string>& locations);
	void showOpenDialog();
	void showQuitDialog();
	void showError(rhea::ErrorReport report);
	void showAlert(std::string text);
	void showDetailsDialog(rhea::ErrorReport report);
	void setStatusText(const std::string& text);

	void navigateOpenDialogTo(const std::string& location);

	void setOpenHandler(OpenHandler handler) { m_open_handler = handler; }
	void onContentOpened(const std::string& address, bool mark = true);

	void setLoading(bool loading);

private:

	bool onQuit(rhea::Event* ev, rhea::Unit* unit);
	bool onQuitCancel(rhea::Event* ev, rhea::Unit* unit);
	bool onAlertClose(rhea::Event* ev, rhea::Unit* unit);

	rhea::Ref<rhea::SceneResource> m_browser_ref;
	rhea::GUIController* m_gui_controller = nullptr;
	OpenBrowser* m_open_browser = nullptr;

	OpenBrowser* m_realm_dialog = nullptr;
	AlertDialog* m_alert_dialog = nullptr;
	rhea::Dialog* m_details_dialog = nullptr;
	rhea::Dialog* m_quit_dialog = nullptr;

	bool m_is_loading = false;

	OpenHandler m_open_handler;

	BrowserController* m_browser_controller = nullptr;
};