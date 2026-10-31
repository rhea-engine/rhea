#include "OpenBrowser.h"
#include <Psio/Log.h>
#include <Rhea/GUI/TextInput.h>
#include <Rhea/Application.h>
#include <Orion/ResourceAddress.h>
#include <algorithm>

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

	const auto colon = value.find(':');
	const auto separator = value.find_first_of("/?#");

	const bool has_scheme =	colon != std::string::npos && (separator == std::string::npos || colon < separator);
	const bool is_absolute = has_scheme || value.compare(0, 2, "//") == 0;

	if (!is_absolute) {
		std::string base = m_current_location;

		if (!base.empty() && base.back() != '/')
			base += '/';

		value = base + value;
	}
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
	const auto colon = value.find(':');
	const auto separator = value.find_first_of("/?#");

	const bool has_scheme =	colon != std::string::npos && (separator == std::string::npos || colon < separator);
	const bool is_absolute = has_scheme || value.compare(0, 2, "//") == 0;

	if (!is_absolute) {
		std::string base = m_current_location;

		if (!base.empty() && base.back() != '/')
			base += '/';

		value = base + value;
	}
	m_selected_address = value;
	setLocationField(m_selected_address);
	navigateTo(m_selected_address);

	return false;
}

bool OpenBrowser::onNavigateUp(rhea::Event* ev, rhea::Unit* unit)
{
	ev->stopPropagation();

	if (m_current_location.empty() ||
		m_current_location == "//recent/" ||
		m_current_location == "//realms/" ||
		m_current_location == "file:///") {
		return false;
	}

	std::string uri = m_current_location;

	// Temporary support for existing browser places.
	if (uri.compare(0, 7, "//file/") == 0)
		uri = "file:///" + uri.substr(7);
	else if (uri.compare(0, 2, "//") == 0)
		uri = "realm:" + uri;

	auto address = orion::ResourceAddress::parse(uri);
	if (!address.success) {
		if (m_status_label)
			m_status_label->setText("Invalid location: " + address.error.message);
		return false;
	}

#if defined(_WIN32)
	if (address.address.scheme() == orion::AddressScheme::File &&
		address.address.authority().empty()) {
		const std::string path = address.address.path();

		const bool drive_root =
			path.size() == 4 &&
			path[0] == '/' &&
			((path[1] >= 'A' && path[1] <= 'Z') || (path[1] >= 'a' && path[1] <= 'z')) &&
			path[2] == ':' &&
			path[3] == '/';

		if (drive_root) {
			navigateTo("file:///");
			return false;
		}
	}
#endif

	auto parent = address.address.parent();
	if (!parent.success) {
		if (m_status_label)
			m_status_label->setText("Cannot navigate up: " + parent.error.message);
		return false;
	}

	// Realm, HTTP and share roots have no further parent.
	if (!(parent.address == address.address))
		navigateTo(parent.address.toString());

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

#if defined(_WIN32)
	if (location == "file:///") {
		m_current_realm = "file:///";
		m_current_location = "file:///";
		m_selected_address.clear();
		setLocationField("file:///");
		m_entries.clear();

		for (std::string drive : psio::Fs::getLogicalDrives()) {
			std::replace(drive.begin(), drive.end(), '\\', '/');

			if (!drive.empty() && drive.back() != '/')
				drive += '/';

			m_entries.push_back({
				drive,                 // Anzeige: C:/
				"file:///" + drive,     // Ziel: file:///C:/
				BrowserPlaceKind::Location
				});
		}

		if (m_status_label)
			m_status_label->setText("");

		update();
		return;
	}
#endif

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
		//search via orion,
		std::string uri = location;
		if (uri.substr(0,7)=="//file/")
			uri = "file:///" + uri.substr(7);

		if (uri.substr(0, 2) == "//") {
			uri = "realm:" + uri;
		}
		orion::AddressResult addr_result = orion::ResourceAddress::parse(uri);
		if (!addr_result.success) {
			DLOG(LOG_DEBUG) << "OpenBrowser: invalid location '" << uri << "'";
			if (m_status_label)
				m_status_label->setText("Invalid location: '" + uri + "'");
			return;
		}

		orion::ResourceRequest* req = m_orion->getResource(addr_result.address);
		if (!req) {
			LOG(LOG_ERROR) << "OpenBrowser: failed to create resource request for location '" << uri << "'";
			if (m_status_label) {
				m_status_label->setText("Failed to create resource request for location: " + uri);
			}
			return;
		}

		req->resolve([this, uri, generation](orion::ResourceRequest* req, orion::Error err) {
			if (generation != m_navigation_generation) {
				DLOG(LOG_DEBUG) << "OpenBrowser: ignoring outdated navigation request for location '" << uri << "'";
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
						m_open_handler(uri);
						return;
					}
				}
				setLocationField(uri);
				m_current_location = uri;
				m_selected_address = m_current_location;
				update();
			}
			else {
				LOG(LOG_ERROR) << "OpenBrowser: failed to resolve location '" << uri << "' with error " << static_cast<int>(err);
				if (m_status_label) {
					switch (err) {
					case orion::Error::RESOURCE_NOT_FOUND:
						m_status_label->setText("Location not found: " + uri);
						break;
					case orion::Error::DATA_INVALID:
						m_status_label->setText("Invalid data at location: " + uri);
						break;
					case orion::Error::NO_AUTH:
					case orion::Error::NO_ACCESS:
						m_status_label->setText("No authorization to access location: " + uri);
						break;
					default:
						m_status_label->setText("Failed to load " + uri);
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
		if (world.empty())
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
	for (const auto& place : m_places) {
		if (place.address.empty())
			continue;

		const std::string label = place.name.empty() ? place.address : place.name;

		m_realms_list->addItem(label, place.address);
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
