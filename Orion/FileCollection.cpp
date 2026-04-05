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
		if (m_handle && !m_end) {
			Entry* entry = nullptr;
			psio::Fs::FileAttributes attributes;
			int ret = psio::Fs::readEntry(m_handle.get(), &attributes);
			if (!ret) {
				entry = new Entry();
				entry->name = attributes.name;
				entry->length = attributes.size;
				if (!psio::Fs::nextEntry(m_handle.get())) {
					m_end = true;
					m_handle.reset();
				}
				return entry;
			}
		}
		return nullptr;
	}

	void FileCollection::Close()
	{
		m_handle.reset();
	}
}