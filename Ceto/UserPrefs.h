#ifndef USERPREFS_H
#define USERPREFS_H

#include <string>
#include <unordered_map>
#include <memory>

#include <ThirdParty/json/single_include/nlohmann/json.hpp>
using json = nlohmann::json;


class UserPrefs
{
public:
	UserPrefs() {};
	UserPrefs(const UserPrefs& e);

	// return UserPrefs as json
	virtual std::string toJSON();

	std::vector<std::string>& lastUsed() { return m_lastused; }
	void lastUsed(std::string lastuse);

	std::vector<std::string>& bookmarks() { return m_bookmarks; }
	void bookmark(std::string bookmark);

protected:
	friend void to_json(json& j, const UserPrefs& e);
	friend void from_json(const json& j, UserPrefs& p);
	std::vector<std::string> m_lastused;
	std::vector<std::string> m_bookmarks;
};
#endif // !USERPREFS_H
