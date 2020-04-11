#include "Entry.h"

#include <ThirdParty/json/single_include/nlohmann/json.hpp>
using json = nlohmann::json;

void to_json(json& j, const Entry& e) {
    j = json{ {"name", e.m_name}, {"type", e.m_content_type}, {"attributes", e.m_values} };
}

void from_json(const json& j, Entry& e) {
    j.at("name").get_to(e.m_name);
    j.at("type").get_to(e.m_content_type);
    j.at("attributes").get_to(e.m_values);
}

Entry::Entry(const Entry& e)
{
    m_name = e.m_name;
    m_content_type = e.m_content_type;
    for (const auto& val : e.m_values)
    {
        m_values[val.first] = val.second;
    }
}

std::string Entry::toJSON()
{
	json j = *this;
    return j.dump(4);
}
