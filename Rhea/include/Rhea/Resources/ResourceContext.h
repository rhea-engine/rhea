#ifndef RHEA_RESOURCECONTEXT_H
#define RHEA_RESOURCECONTEXT_H

#include <string>
#include <Orion/ResourceAddress.h>

namespace rhea
{
    struct ResourceContext {
        std::string context_root;
        std::string source_directory;

		// normalize rid to absolute path based on context_root and source_directory
		bool normalizeResourceId(const std::string& rid, std::string& address) const {

			auto fail = [&](const std::string& message) {
				address = message;
				return false;
				};

			auto finish = [&](const orion::AddressResult& result) {
				if (!result.success)
					return fail(result.error.message);

				address = result.address.toString();
				return true;
				};

			if (rid.empty()) {
				return fail("error: Resource ID is empty");
			}

			if (rid.compare(0, 2, "//") == 0) {
				return finish(orion::ResourceAddress::parse("realm:" + rid));
			}

			// A colon before any path/query/fragment separator indicates
			// a scheme. Let the parser validate it, including unknown schemes.
			const auto colon = rid.find(':');
			const auto separator = rid.find_first_of("/?#");
			const bool has_scheme = colon != std::string::npos && (separator == std::string::npos || colon < separator);

			// already normalized?
			if (has_scheme) {
				return finish(orion::ResourceAddress::parse(rid));
			}
			const bool project_relative = rid.front() == '/';
			const std::string& base_text = project_relative ? context_root : source_directory;

			if (base_text.empty())
				return fail("Resource reference requires a base directory");

			auto base = orion::ResourceAddress::parse(base_text);
			if (!base.success)
				return fail(base.error.message);

			if (base.address.path().back() != '/' ||
				base_text.find_first_of("?#") != std::string::npos) {
				return fail("Resource context requires a directory address");
			}

			const std::string relative = project_relative ? rid.substr(1) : rid;

			return finish(base.address.resolveRelative(relative));
		}
    };
}
#endif // !RHEA_RESOURCECONTEXT_H