/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/FileCollection.h>

namespace orion
{
	FileCollection::FileCollection(std::string path) :
		m_end(false)
	{
		m_handle.reset(psio::Fs::openDirectory(path));
	}

	FileCollection::~FileCollection()
	{
	}

	Collection::Entry* FileCollection::nextEntry()
	{
		while (m_handle && !m_end) {
			Entry* entry = nullptr;
			psio::Fs::FileAttributes attributes;
			int ret = psio::Fs::readEntry(m_handle.get(), &attributes);
			if (!ret) {
				entry = new Entry();
				entry->name = attributes.name;
				entry->length = attributes.size;
				entry->is_dir = attributes.is_dir;
				if (!psio::Fs::nextEntry(m_handle.get())) {
					m_end = true;
					m_handle.reset();
				}
				if (m_filter) {
					if (!m_filter(*entry)) {
						delete entry;
						continue;
					}
				}
				return entry;
			}
			else
				return nullptr;
		}
		return nullptr;
	}

	void FileCollection::Close()
	{
		m_handle.reset();
	}
}