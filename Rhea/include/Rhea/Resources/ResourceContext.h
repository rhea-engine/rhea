#ifndef RHEA_RESOURCECONTEXT_H
#define RHEA_RESOURCECONTEXT_H

#include <string>

namespace rhea
{
    struct ResourceContext {
        std::string context_root;
        std::string source_directory;
		// normalize rid to absolute path based on context_root and source_directory
		std::string normalizeResourceId(const std::string& rid) const {
			if (rid.compare(0, 2, "//") == 0) {
				return rid; // already absolute
			}
			else if (rid.compare(0, 1, "/") == 0) {
				return context_root + rid; // relative to context root
			}
			else if (source_directory.empty()) {
				return rid; // no source directory, return as is
			}
			else {
				return source_directory + "/" + rid; // relative to source directory
			}
		}
    };
}
#endif // !RHEA_RESOURCECONTEXT_H