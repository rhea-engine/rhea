/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Fs/Fs.h>
#ifdef WIN32
#include <direct.h>
#endif
#include <Psio/Log.h>

namespace psio
{
	bool Fs::removeDirectoryTree(const std::string& path)
	{
		DirectoryHandle* handle = openDirectory(path);
		if (!handle) {
			LOG(LOG_ERROR) << "removeDirectoryTree: Failed to open directory: " << path;
			return false;
		}
		if (handle->hasEntries()) {
			FileAttributes attributes;
			while (readEntry(handle, &attributes) == 0) {
				if (attributes.name == "." || attributes.name == "..")
					continue;
				std::string child_path = path + "/" + attributes.name;
				if (attributes.is_dir) {
					if (!removeDirectoryTree(child_path)) {
						LOG(LOG_ERROR) << "removeDirectoryTree: Failed to remove directory: " << child_path;
						return false;
					}
				}
				else {
					if (!removeFile(child_path)) {
						LOG(LOG_ERROR) << "removeDirectoryTree: Failed to remove file: " << child_path;
						return false;
					}
				}
			}
		}
		return removeDirectory(path);
	}
}