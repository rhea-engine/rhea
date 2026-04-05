#ifndef RHEA_RESOURCES_COLLECTION_H
#define RHEA_RESOURCES_COLLECTION_H

#include "../Unit.h"
#include <Orion/ResourceStream.h>

namespace rhea
{
	class Collection : public Unit
	{
		MECLASS(Collection, Unit)
	public:
		class Entry {
		public:
			std::string name;
			std::string mime;
			size_t length;
		};

		Collection(std::string name, std::string type_name) :
			m_idx(0),
			Unit(name, type_name)
		{}

		void readFromStream(orion::ResourceStream* stream);

		void begin() { m_idx = 0; }
		bool end() { return m_idx > m_entries.size(); }
		Entry getNext();

	protected:
		std::vector<Entry> m_entries;
		int m_idx;
	};
}

#endif // !RHEA_RESOURCES_COLLECTION_H
