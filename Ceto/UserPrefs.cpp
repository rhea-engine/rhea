#include "UserPrefs.h"

void to_json(json& j, const UserPrefs& e)
{
	j = json{ {"lastused", e.m_lastused}, {"bookmarks", e.m_bookmarks} };
}

void from_json(const json& j, UserPrefs& e)
{
	try {
		j.at("lastused").get_to(e.m_lastused);
	}
	catch (json::out_of_range& e) {}
	try {
		j.at("bookmarks").get_to(e.m_bookmarks);
	}
	catch (json::out_of_range& e) {}
}

UserPrefs::UserPrefs(const UserPrefs& e)
{
	m_lastused = e.m_lastused;
	m_bookmarks = e.m_bookmarks;
}

std::string UserPrefs::toJSON()
{
	json j = *this;
	return j.dump(4);
}

void UserPrefs::lastUsed(std::string lastuse)
{
	// remove if already there
	auto it = std::find(m_lastused.begin(), m_lastused.end(), lastuse);
	if (it != m_lastused.end()) {
		m_lastused.erase(it);
	}
	// insert at front
	m_lastused.insert(m_lastused.begin(), lastuse);
}

void UserPrefs::bookmark(std::string bookmark)
{
	// insert if not existing (todo)
	if (!m_bookmarks.size() || m_bookmarks[0] != bookmark) {
		m_bookmarks.push_back(bookmark);
	}
}
