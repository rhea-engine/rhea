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
		Collection(),
		m_handle(nullptr)
	{
		m_handle.reset(psio::Fs::openDirectory(path));
		m_state = m_handle ? StreamState::STREAM_OPEN : StreamState::STREAM_ERROR;
	}

	FileCollection::~FileCollection()
	{
	}

	FileCollection::FileCollection(std::vector<std::string> directories) :
		m_directories(std::move(directories))
	{
		m_state = StreamState::STREAM_OPEN;
	}

	Collection::Entry* FileCollection::nextEntry()
	{
		while (m_state == StreamState::STREAM_OPEN && m_directory_index < m_directories.size()) {
			Entry* entry = new Entry();
			entry->name = m_directories[m_directory_index++];
			entry->is_dir = true;
			if (m_directory_index == m_directories.size())
				Close();
			if (m_filter && !m_filter(*entry)) {
				delete entry;
				continue;
			}
			return entry;
		}
		if (!m_handle) {
			Close();
			return nullptr;
		}
		while (m_handle && m_state == StreamState::STREAM_OPEN) {
			Entry* entry = nullptr;
			psio::Fs::FileAttributes attributes;
			int ret = psio::Fs::readEntry(m_handle.get(), &attributes);
			if (!ret) {
				entry = new Entry();
				entry->name = attributes.name;
				entry->length = attributes.size;
				entry->is_dir = attributes.is_dir;
				if (!psio::Fs::nextEntry(m_handle.get())) {
					Close();
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
		m_state = StreamState::STREAM_CLOSED;
	}
}
