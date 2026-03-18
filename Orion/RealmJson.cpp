#include "RealmJson.h"

#include <ctime> // for std::time

namespace Orion
{
    void to_json(Json& j, const Realm& r)
    {
        j = Json{
            {"name", r.getName()},
            {"type", r.getContentType()},
            {"href", r.getResourceUrl()},
            {"ttl",  r.getTTL()}
        };
    }

    void from_json(const Json& j, Realm& r)
    {
        // Use temporary strings so we can set via setters
        std::string name, type, href;
        std::time_t ttl{};

        j.at("name").get_to(name);
        j.at("type").get_to(type);
        j.at("href").get_to(href);
        j.at("ttl").get_to(ttl);

        r = Realm(std::move(name), std::move(href), ttl);
        r.setContentType(std::move(type));

        // "ttl is delta"
        r.setTTL(ttl + std::time(nullptr));
    }
}