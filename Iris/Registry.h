/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef REGISTRY_H
#define REGISTRY_H

#include <string>
#include <functional>
#include "Entry.h"

class Registry
{
public:

	typedef std::function<int(Entry*)> EntryHandler;

	// lookup an entry and return success or error code
	virtual int lookupEntry(std::string path, EntryHandler handler) = 0;
	
	// insert an entry
	virtual void insert(std::string path, Entry* entry) = 0;

	// load entries from file and add it to the database returning number of entries
	virtual int importFromFile(std::string filename) = 0;
};

#endif // !REGISTRY_H
