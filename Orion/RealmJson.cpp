/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "RealmJson.h"

#include <ctime> // for std::time
#include <stdexcept>
#include <Psio/Net/UrlRequest/Url.h>

namespace orion
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
       
        if (j.at("ttl").is_number_integer()) {
            j.at("ttl").get_to(ttl);
            // test if the ttl is between 0 and 86400 and throw an error if it is not
            if (ttl < 0 || ttl > 86400) {
                throw std::invalid_argument("TTL must be between 0 and 86400 seconds");
            }        
        }
        else {
			throw std::invalid_argument("TTL must be an integer");
        }

		// test if the href is a valid url
		psio::Url url(href);
		if (!url.isValid() || 
            url.hostname().empty() || 
            url.scheme().empty() || 
            url.scheme() != "https" ||
            url.hasFragment() ||
            url.hasQuery() || 
            url.path().empty() || url.path().back() != '/') {
			throw std::invalid_argument("Invalid URL: " + href);
		}
        
        // check if the type is collection
		if (type != "collection") {
			throw std::invalid_argument("Invalid type: " + type + ". Only 'collection' is supported.");
		}

        r = Realm(std::move(name), std::move(href), ttl);
        r.setContentType(std::move(type));

        // "ttl is delta"
        r.setTTL(ttl + std::time(nullptr));
    }
}