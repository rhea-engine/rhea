#ifndef ENTRY_H
#define ENTRY_H

#include <string>
#include <unordered_map>
#include <memory>

#include <nlohmann/json.hpp>
using json = nlohmann::json;


class Entry
{
public:
	Entry(std::string name, std::string content_type) :
		m_name(name),
		m_content_type(content_type)
	{}
	Entry(const Entry& e);
	// return entry as json
	virtual std::string toJSON();
	// set attribute value
	void setValue(std::string key, std::string value) { m_values[key] = value; }
protected:
	friend void to_json(json& j, const Entry& e);
	friend void from_json(const json& j, Entry& p);
	std::string m_name;
	std::string m_content_type;
	std::unordered_map<std::string, std::string> m_values;
};
#endif // !ENTRY_H
