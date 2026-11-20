#include "CetoUI.h"

#include <Rhea/Core/Scene.h>
#include <Rhea/Core/SceneResource.h>
#include <Rhea/Application.h>
#include <Rhea/UnitTree.h>
#include <Psio/Log.h>
#include "../BrowserController.h"

void CetoUI::setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values)
{
	rhea::Item::setup(app, attributes, values);
	if (m_browser_ref.id().empty()) {
		m_browser_ref.setId("//ceto/openbrowser.scene");
	}
}

void CetoUI::ready()
{
    Item::ready();

	rhea::Scene* scene = doCast<rhea::Scene>(m_parent);
	if (!scene)
		return;
	m_gui_controller = doCast<rhea::GUIController>(scene->find("gui"));
   

	// quit dialog
	m_quit_dialog = doCast<rhea::Dialog>(m_gui_controller->find("quit"));
	if (m_quit_dialog) {
		m_quit_dialog->registerEventHandler("submitted", this, std::bind(&CetoUI::onQuit, this, std::placeholders::_1, std::placeholders::_2));
		m_quit_dialog->registerEventHandler("canceled", this, std::bind(&CetoUI::onQuitCancel, this, std::placeholders::_1, std::placeholders::_2));
	}
	scene->registerEventHandler("quit", this, std::bind(&CetoUI::onQuit, this, std::placeholders::_1, std::placeholders::_2));

	m_alert_dialog = doCast<AlertDialog>(m_gui_controller->find("alert"));
	if (!m_alert_dialog) {
		LOG(LOG_ERROR) << "no alert dialog found";
	}

	m_details_dialog = doCast<rhea::Dialog>(m_gui_controller->find("details"));
	if (!m_details_dialog) {
		LOG(LOG_ERROR) << "no details dialog found";
	}
	else {
		m_details_dialog->registerEventHandler("submitted", this, [this](rhea::Event* ev, rhea::Unit* unit) {
			m_gui_controller->closeDialog(m_details_dialog);
			return false;
			});
		m_details_dialog->registerEventHandler("canceled", this, [this](rhea::Event* ev, rhea::Unit* unit) {
			m_gui_controller->closeDialog(m_details_dialog);
			return false;
			});
	}

	m_alert_dialog = doCast<AlertDialog>(m_gui_controller->find("alert"));
	if (!m_alert_dialog) {
		LOG(LOG_ERROR) << "no alert dialog found";
	}
	else {
		scene->registerEventHandler("close_alert", this, std::bind(&CetoUI::onAlertClose, this, std::placeholders::_1, std::placeholders::_2));
	}
}

void CetoUI::initializeBrowser(BrowserController* controller)
{
	auto* browser_resource = m_app->getUnitTree()->resolve(m_browser_ref);

	if (!browser_resource || !m_gui_controller)
		return;

	rhea::ErrorReport error;
	auto* instance = browser_resource->spawnPrepared(m_gui_controller, "openbrowser", &error);

	m_open_browser = doCast<OpenBrowser>(instance);
	if (error.isError() || !m_open_browser) {
		LOG(LOG_ERROR) << "Failed to spawn OpenBrowser: " << (error.errors.size() ? error.errors[0].getMessage() : "Unknown error");
		return;
	}

	m_open_browser->setOpenHandler([this](const std::string& address) {
		if (m_open_handler)
			m_open_handler(address);
		});
	m_open_browser->ready();

	m_browser_controller = controller;
	m_browser_controller->setLoadingHandler(std::bind(&CetoUI::setLoading, this, std::placeholders::_1));
	m_browser_controller->addOpenedHandler(std::bind(&CetoUI::onContentOpened, this, std::placeholders::_1, std::placeholders::_2));
	m_browser_controller->setErrorHandler(std::bind(&CetoUI::showError, this, std::placeholders::_1));
}

std::vector<rhea::Unit::ResourceReference> CetoUI::getReferences()
{
    auto refs = Item::getReferences();
    refs.push_back({
        "browser",
        "//meworla/sceneresource",
        m_browser_ref.id(),
        ResourceUsage::Prepare
        });
    return refs;
}

void CetoUI::onContentOpened(const std::string& address, bool mark)
{
	m_gui_controller->closeDialog(m_open_browser);
	
	// hide GUI background
	rhea::GUIController* w = doCast<rhea::GUIController>(m_parent->find("gui"));
	if (w) {
		w->setStyle(rhea::StylePropertyId::kBackgroundImage, "none");
		w->UpdateStyle();
	}
}

void CetoUI::setBrowserData(const BrowserData& data)
{
	m_open_browser->setPlaces(data.places);
	m_open_browser->setRecentWorlds(data.recent_locations);
}

void CetoUI::setRecentLocations(const std::vector<std::string>& locations)
{
	m_open_browser->setRecentWorlds(locations);
}

void CetoUI::showOpenDialog()
{
	if (m_gui_controller && m_open_browser) {
		m_gui_controller->openDialog(m_open_browser);
	}
}

void CetoUI::showQuitDialog()
{
	if (m_gui_controller && m_quit_dialog) {
		m_gui_controller->openDialog(m_quit_dialog);
	}
}

bool CetoUI::onQuit(rhea::Event* ev, rhea::Unit* unit)
{
	sendEvent("quit_application", ev);
	return false;
}

bool CetoUI::onQuitCancel(rhea::Event* ev, rhea::Unit* unit)
{
	if (m_gui_controller && m_quit_dialog) {
		m_gui_controller->closeDialog(m_quit_dialog);
	}
	return false;
}

void CetoUI::showError(rhea::ErrorReport report)
{
	AlertDialog::Content content;
	content.title = "Could not open world";
	if (report.errors.size() == 1) {
		rhea::Error err = report.errors[0];
		content.message = err.getMessage() + "\n" + err.getResourceId();
		content.actions = {
			{ "ok", "OK", true }
		};
	}
	else {
		content.message = std::to_string(report.errors.size()) + " problems occurred while loading this world.";
		content.actions = {
			{ "details", "Details...", false },
			{ "ok", "OK", true }
		};
	}
	content.default_action = "ok";
	content.cancel_action = "ok";

	if (m_alert_dialog->configure(std::move(content), [this, report](std::string action_id) {
		if (action_id == "details")
			showDetailsDialog(report);
		}))
	{
		m_gui_controller->openDialog(m_alert_dialog);
	}
}

void CetoUI::showDetailsDialog(rhea::ErrorReport report)
{
	if (!m_gui_controller || !m_details_dialog)
		return;

	std::string title = "Could not open world";
	rhea::Label* title_label = doCast<rhea::Label>(m_details_dialog->findByName("title"));
	if (title_label) {
		title_label->setText(title);
	}

	rhea::Label* message_label = doCast<rhea::Label>(m_details_dialog->findByName("message"));
	if (message_label) {
		message_label->setText("The world could not be opened due to the following errors:");
	}

	std::string message;
	for (auto& err : report.errors) {
		message += "\n\n" + err.getMessage() + "\n" + err.getResourceId();
	}

	rhea::Label* label = doCast<rhea::Label>(m_details_dialog->findByName("details_text"));
	if (label) {
		label->setText(message);
	}

	m_gui_controller->openDialog(m_details_dialog);
}

void CetoUI::setStatusText(const std::string& text)
{
	if (m_open_browser) {
		m_open_browser->setStatus(text);
	}
}

void CetoUI::navigateOpenDialogTo(const std::string& location)
{
	if (m_gui_controller && m_open_browser) {
		m_open_browser->navigateTo(location);
	}
}

void CetoUI::setLoading(bool loading)
{
	if (m_is_loading == loading)
		return;

	m_is_loading = loading;
	if (loading)
		setStatusText("Loading...");
	else
		setStatusText("");
}

void CetoUI::showAlert(std::string text)
{
	if (!m_gui_controller || !m_alert_dialog)
		return;

	rhea::Label* label = doCast<rhea::Label>(m_alert_dialog->findByName("info"));
	if (!label)
		return;

	// set the text
	label->setText(text);

	// show dialog
	m_gui_controller->openDialog(m_alert_dialog);
}

bool CetoUI::onAlertClose(rhea::Event* ev, rhea::Unit* unit)
{
	if (m_gui_controller && m_alert_dialog)
		m_gui_controller->closeDialog(m_alert_dialog);

	return false;
}