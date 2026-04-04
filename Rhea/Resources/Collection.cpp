#include "Rhea/Resources/Collection.h"
#include <Orion/Collection.h>

namespace rhea
{
	void Collection::readFromStream(orion::ResourceStream* stream)
	{
		if (!stream->IsDir())
			return;
	
		orion::Collection* col = static_cast<orion::Collection*>(stream);
		orion::Collection::Entry* stream_entry;
		
		while (stream_entry = col->nextEntry()) {
			m_entries.push_back({stream_entry->name, stream_entry->mime, stream_entry->length });
		}
	}

	Collection::Entry Collection::getNext()
	{
		Entry res;

		if (m_idx < m_entries.size()) {
			res = m_entries[m_idx];
			m_idx++;			
		}
		return res;
	}
}