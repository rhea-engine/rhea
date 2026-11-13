#include <Orion/JsonCollection.h>
#include <nlohmann/json.hpp>
#include <Psio/Log.h>

namespace orion
{
	JsonCollection::JsonCollection(char* buf, size_t buflen, const orion::ResourceAddress& effective_address)
	{
		nlohmann::json json;
		try {
			json = nlohmann::json::parse(buf, buf + buflen);
			
			if (!json.is_object())
				throw std::runtime_error("Root must be an object");

			// is it a catalog?
			if (json.at("format").get<std::string>() != "orion-catalog") {
				m_parse_error = Error::NOT_IMPLEMENTED;
				return;
			}

			if (json.at("version").get<int>() != 1)
				throw std::runtime_error("Unsupported collection version");

			const auto& entries = json.at("entries");

			if (!entries.is_array())
				throw std::runtime_error("'entries' must be an array");

			std::vector<Entry> parsed_entries;

			for (const auto& item : entries) {
				if (!item.is_object())
					throw std::runtime_error("Entry must be an object");

				Entry entry;
				entry.name = item.at("name").get<std::string>();

				const auto href = item.at("href").get<std::string>();
				const auto kind = item.at("kind").get<std::string>();

				if (href.empty())
					throw std::runtime_error("Entry href must not be empty");

				if (entry.name.empty())
					entry.name = href;

				if (kind != "world" && kind != "directory" && kind != "file")
					throw std::runtime_error("Unsupported entry kind");

				entry.is_dir = kind == "directory";
				entry.mime = item.value("mime_type", std::string{});

				auto target = effective_address.resolveRelative(href);
				if (!target.success) {
					throw std::runtime_error("Failed to resolve href: " + target.error.message);
				}
				entry.address = target.address.toString();
				if (entry.is_dir &&	!entry.address.empty() && entry.address.back() != '/')
				{
					entry.address += '/';
				}

				parsed_entries.push_back(std::move(entry));
			}

			m_entries = std::move(parsed_entries);
			m_parse_error = Error::OK;
		}
		catch (const std::exception& e) {
			m_parse_error = Error::DATA_INVALID;
			LOG(LOG_ERROR) << "JSON collection is invalid: " << e.what() << ", " << effective_address.toString();
		}
	}

    Collection::Entry* JsonCollection::nextEntry()
    {
        while (m_next_entry < m_entries.size()) {
            Entry* entry = &m_entries[m_next_entry++];

            if (!m_filter || m_filter(*entry))
                return entry;
        }

        return nullptr;
    }

	Error JsonCollection::parse(char* buf, size_t buflen, const orion::ResourceAddress& effective_address,
		std::unique_ptr<JsonCollection>& collection)
	{
		try {
			collection.reset(new JsonCollection(buf, buflen, effective_address));
		}
		catch (const std::exception& e) {
			return Error::DATA_INVALID;
		}
		return collection->m_parse_error;
	}
}