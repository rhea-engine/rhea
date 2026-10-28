#include "OpenBrowser.h"
#include <Psio/Log.h>
#include <Rhea/GUI/TextInput.h>
#include <Rhea/Application.h>

void  OpenBrowser::setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values)
{
	Dialog::setup(app, attributes, values);
	// setup the dialog
	setModal(true);
}

void OpenBrowser::ready()
{
	Dialog::ready();

	m_realms_list = doCast<rhea::ListWidget>(find("content/layout/realms/scroll/list"));
	m_list = doCast<rhea::ListWidget>(find("content/layout/entries/scroll/list"));
	m_location_input = doCast<rhea::TextInput>(findByName("location_input"));
	m_status_label = doCast<rhea::Label>(findByName("status_label"));

	m_orion = m_app->getFS();

	// register action handlers for the dialog
	if (!m_list || !m_realms_list)
		return;
	m_list->registerEventHandler("selection_changed", this, std::bind(&OpenBrowser::onEntrySelect, this, std::placeholders::_1, std::placeholders::_2));
	m_list->registerEventHandler("item_activated", this, std::bind(&OpenBrowser::onEntryActivated, this, std::placeholders::_1, std::placeholders::_2));

	registerEventHandler("search", this, std::bind(&OpenBrowser::onRealmSearch, this, std::placeholders::_1, std::placeholders::_2));
	m_realms_list->registerEventHandler("item_clicked", this, std::bind(&OpenBrowser::onRealmSelect, this, std::placeholders::_1, std::placeholders::_2));
	m_realms_list->registerEventHandler("item_activated", this, std::bind(&OpenBrowser::onRealmSelect, this, std::placeholders::_1, std::placeholders::_2));
	registerEventHandler("navigate_up", this, std::bind(&OpenBrowser::onNavigateUp, this, std::placeholders::_1, std::placeholders::_2));
	registerEventHandler("open", this, std::bind(&OpenBrowser::onOpen, this, std::placeholders::_1, std::placeholders::_2));
}

bool OpenBrowser::onEntrySelect(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();

	rhea::Unit* target = ev->getTarget();
	if (!target) {
		DLOG(LOG_DEBUG) << "onEntrySelect: target is null";
		return false;
	}
	std::string  value = (std::string)target->getValue("value");

	if (m_current_location.back() != '/')
		m_current_location += '/';

	// if value is relative add current location
	if (value.compare(0, 2, "//") != 0)
		value = m_current_location + value;
	m_selected_address = value;
	setLocationField(m_selected_address);

	return false;
}

bool OpenBrowser::onEntryActivated(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();
	rhea::Unit* target = ev->getTarget();
	if (!target) {
		DLOG(LOG_DEBUG) << "onEntryActivated: target is null";
		return false;
	}
	std::string value = (std::string)target->getValue("value");
	if (m_current_location.back() != '/')
		m_current_location += '/';
	// if value is relative add current location
	if (value.compare(0, 2, "//") != 0)
		value = m_current_location + value;
	m_selected_address = value;
	setLocationField(m_selected_address);
	navigateTo(m_selected_address);

	return false;
}

bool OpenBrowser::onNavigateUp(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();

	// if current location is not only a realm, navigate to the parent directory
	if (m_current_location.length() > 2) {
		// remove trailing slash if present
		std::string location = m_current_location;
		if (location.back() == '/')
			location.pop_back();
		// find last slash
		size_t last_slash = location.find_last_of('/');
		if (last_slash > 1 && last_slash != std::string::npos) {
			std::string parent_location = location.substr(0, last_slash + 1);
			navigateTo(parent_location);
		}
	}

	return false;
}

bool OpenBrowser::onOpen(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();

	if (!m_location_input) {
		DLOG(LOG_DEBUG) << "OpenBrowser: location input is null";
		return false;
	}

	std::string address = m_location_input->getText();
	if (address.empty()) {
		address = m_selected_address;
	}
	m_selected_address = address;
	navigateTo(address);

	return false;
}

bool OpenBrowser::onRealmSearch(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();
	rhea::TextInput* target = doCast<rhea::TextInput>(ev->getTarget());
	if (!target) {
		DLOG(LOG_DEBUG) << "onRealmSearch: target is not a TextInput";
		return false;
	}
	std::string value = (std::string)target->getText();
	m_open_handler(value);

	return false;
}

bool OpenBrowser::onRealmSelect(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();

	rhea::Unit* target = ev->getTarget();
	if (!target) {
		DLOG(LOG_DEBUG) << "onRealmSelect: target is null";
		return false;
	}
	std::string  value = (std::string)target->getValue("value");

	navigateTo(value);
	return false;
}

std::string OpenBrowser::getRealmName(const std::string& address)
{
	if (address.compare(0, 2, "//") != 0)
		return {};

	const auto realm_end = address.find('/', 2);
	return address.substr(2, realm_end == std::string::npos
		? std::string::npos
		: realm_end - 2);
}

void OpenBrowser::setOpenHandler(OpenHandler handler)
{
	m_open_handler = handler;
}

void OpenBrowser::setRecentWorlds(const std::vector<std::string>& worlds)
{
	m_recent_worlds = worlds;
}

void OpenBrowser::setPlaces(const std::vector<BrowserPlace>& places)
{
	m_places = places;
	updateRealms();
}

void OpenBrowser::navigateTo(const std::string& location)
{
	const auto generation = ++m_navigation_generation;

	// get realm part of location
	std::string realm_name = getRealmName(location);
	const BrowserPlace* place = nullptr;
	std::size_t best_length = 0;
	for (const auto& p : m_places) {
		std::string prefix = p.address;
		while (prefix.size() > 2 && prefix.back() == '/')
			prefix.pop_back();

		if (prefix.size() <= 2)
			continue;

		const bool matches =
			location.compare(0, prefix.size(), prefix) == 0 &&
			(location.size() == prefix.size() ||
				location[prefix.size()] == '/');

		if (matches && prefix.size() > best_length) {
			place = &p;
			best_length = prefix.size();
		}
	}

	if (place && place->kind == BrowserPlaceKind::Internal) {
		if (realm_name == "recent") {
			// show recent worlds
			m_current_realm = "//recent/";
			m_current_location = m_current_realm;
			m_selected_address = m_current_location;
			setLocationField("");
			showRecent();
			if (m_status_label) {
				m_status_label->setText("");
			}
		}
		else if (realm_name == "realms") {
			// show list of realms
			m_current_realm = "//realms/";
			m_current_location = m_current_realm;
			m_selected_address = m_current_location;
			setLocationField("");
			showRealms();
			if (m_status_label) {
				m_status_label->setText("");
			}
		}
		else {
			DLOG(LOG_DEBUG) << "OpenBrowser: unknown internal place '" << place->name << "'";
			return;
		}
	}
	else if (!place || place->kind == BrowserPlaceKind::Location) {
		//search via orion
		orion::ResourceRequest* req = m_orion->getResourceByRid(location);
		if (!req) {
			LOG(LOG_ERROR) << "OpenBrowser: failed to create resource request for location '" << location << "'";
			if (m_status_label) {
				m_status_label->setText("Failed to create resource request for location: " + location);
			}
			return;
		}

		if (location.empty() || location.substr(0, 2) != "//" || location.substr(0, 3) == "///") {
			DLOG(LOG_DEBUG) << "OpenBrowser: invalid location '" << location << "'";
			if (m_status_label)
				m_status_label->setText("Invalid location: '" + location + "'");
			return;
		}
		if (m_status_label) {
			m_status_label->setText("Loading " + location + "...");
		}
		req->resolve([this, location, generation](orion::ResourceRequest* req, orion::Error err) {
			if (generation != m_navigation_generation) {
				DLOG(LOG_DEBUG) << "OpenBrowser: ignoring outdated navigation request for location '" << location << "'";
				return;
			}
			if (err == orion::Error::OK) {
				if (m_status_label) {
					m_status_label->setText("");
				}
				m_entries.clear();
				if (req->getStream()->IsDir()) {
					orion::Collection* collection = static_cast<orion::Collection*>(req->getStream());
					int count = 0;
					while (auto* entry = collection->nextEntry()) {
						if (entry->name == "." || entry->name == "..") continue;

						// apply filter
						//if (!shouldShowEntry(entry->name, entry->is_dir)) continue;

						if (entry->is_dir) {
							m_entries.push_back({ entry->name + "/", entry->name + "/", BrowserPlaceKind::Location });
						}
						else {
							m_entries.push_back({ entry->name, entry->name, BrowserPlaceKind::Location });
						}
					}
				}
				else {
					if (m_open_handler) {
						m_open_handler(location);
						return;
					}
				}
				setLocationField(location);
				m_current_location = location;
				m_selected_address = m_current_location;
				update();
			}
			else {
				LOG(LOG_ERROR) << "OpenBrowser: failed to resolve location '" << location << "' with error " << static_cast<int>(err);
				if (m_status_label) {
					switch (err) {
					case orion::Error::RESOURCE_NOT_FOUND:
						m_status_label->setText("Location not found: " + location);
						break;
					case orion::Error::DATA_INVALID:
						m_status_label->setText("Invalid data at location: " + location);
						break;
					case orion::Error::NO_AUTH:
					case orion::Error::NO_ACCESS:
						m_status_label->setText("No authorization to access location: " + location);
						break;
					default:
						m_status_label->setText("Failed to load " + location);
						break;
					}
				}
			}
			});
		return;
	}
	else {
		LOG(LOG_ERROR) << "OpenBrowser: unknown place kind for place '" << place->name << "'";
		return;
	}
	update();
}

void OpenBrowser::showRecent()
{
	m_entries.clear();
	for (auto& world : m_recent_worlds) {
		if (world.empty() || world.substr(0, 2) != "//")
			continue;
		m_entries.push_back({world, world, BrowserPlaceKind::Location});
	}
}

void OpenBrowser::showRealms()
{
	m_entries.clear();
	for (auto& place : m_places) {
		if (place.kind == BrowserPlaceKind::Location) {
			m_entries.push_back(place);
		}
	}
}

void  OpenBrowser::refresh()
{
	navigateTo(m_current_location);
}

void OpenBrowser::updateRealms()
{
	// update the list of realms
	if (!m_realms_list) {
		LOG(LOG_ERROR) << "OpenBrowser: realms list widget not found";
		return;
	}
	m_realms_list->clear();
	for (auto& place : m_places) {
		if (place.address.empty() || place.address.substr(0, 2) != "//" || place.address.substr(0, 3) == "///")
			continue;
		if (place.name.empty())
			place.name = place.address.substr(2, place.address.length() - 2);
		m_realms_list->addItem(place.name, place.address);
	}
}

void OpenBrowser::update()
{
	if (!m_list) {
		LOG(LOG_ERROR) << "OpenBrowser: entries list widget not found";
		return;
	}
	if (m_current_location.empty())
		return;

	m_list->clear();
	// update the list of entries
	for (auto& entry : m_entries) {
		m_list->addItem(entry.name, entry.address);
	}
}

void OpenBrowser::refreshEntries()
{}

void OpenBrowser::applyFilter()
{}

void OpenBrowser::acceptSelection()
{}

void OpenBrowser::setLocationField(const std::string & location)
{
	if (!m_location_input) {
		LOG(LOG_ERROR) << "OpenBrowser: location input widget not found";
		return;
	}
	m_location_input->setText(location);
}
