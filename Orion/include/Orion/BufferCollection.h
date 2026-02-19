#ifndef ORION_BUFFERCOLLECTION_H
#define ORION_BUFFERCOLLECTION_H

#include <vector>

#include <Orion/Collection.h>

namespace Orion
{
	class BufferCollection : public Collection
	{
	protected:
		std::vector<Entry> m_entries;
		int size;
	public:
		BufferCollection();
	};
}
#endif // !ORION_BUFFERCOLLECTION_H
