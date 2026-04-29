/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ORION_BUFFERCOLLECTION_H
#define ORION_BUFFERCOLLECTION_H

#include <vector>

#include <Orion/Collection.h>

namespace orion
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
