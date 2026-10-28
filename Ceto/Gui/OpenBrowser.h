#pragma once

#include <Rhea/GUI/Dialog.h>
#include <Rhea/GUI/List.h>
#include <Rhea/GUI/TextInput.h>
#include <Orion/Orion.h>
#include <vector>
#include <functional>
#include <unordered_map>
#include <string>
#include <cstdint>

class OpenBrowser : public rhea::Dialog
{
	RECLASS(OpenBrowser, rhea::Dialog, "//ceto/gui/openbrowser")
public:
	OpenBrowser(std::string name, std::string type_name) :
		rhea::Dialog(name, type_name)
	{
		defineSlot("content", false);
	}
	void setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, simpleparser::Variant> values) override;
	void ready() override;

	enum class BrowserPlaceKind { Internal, Location };

	struct BrowserPlace
	{
		std::string name;
		std::string address;
		BrowserPlaceKind kind;
	};

	using OpenHandler = std::function<void(const std::string&)>;

	void setOpenHandler(OpenHandler handler);
	std::string getRealmName(const std::string& address);

	void setRecentWorlds(const std::vector<std::string>& worlds);
	void setPlaces(const std::vector<BrowserPlace>& places);

	void navigateTo(const std::string& location);
	void showRecent();
	void showRealms();

	void refresh();
	void update();
	void updateRealms();

private:
	void refreshEntries();
	void applyFilter();
	void acceptSelection();

	void setLocationField(const std::string& location);

	bool onRealmSelect(rhea::Event* ev, rhea::Unit* unit);
	bool onRealmSearch(rhea::Event* ev, rhea::Unit* unit);
	bool onEntrySelect(rhea::Event* ev, rhea::Unit* unit);
	bool onEntryActivated(rhea::Event* ev, rhea::Unit* unit);
	bool onNavigateUp(rhea::Event* ev, rhea::Unit* unit);
	bool onOpen(rhea::Event* ev, rhea::Unit* unit);

	rhea::ListWidget* m_realms_list = nullptr;
	rhea::ListWidget* m_list = nullptr;
	rhea::TextInput* m_location_input = nullptr;
	rhea::Label* m_status_label = nullptr;

	orion::Orion* m_orion = nullptr;

	OpenHandler m_open_handler;
	std::string m_current_realm;
	std::string m_current_location;
	std::string m_selected_address;
	std::vector<std::string> m_recent_worlds;
	std::vector<BrowserPlace> m_places;
	std::vector<BrowserPlace> m_entries;

	std::uint64_t m_navigation_generation = 0;
};