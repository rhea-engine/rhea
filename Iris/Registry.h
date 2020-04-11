#ifndef REGISTRY_H
#define REGISTRY_H

#include <string>
#include <functional>
#include "Entry.h"
#include "IRegistryHandler.h"

class Registry
{
public:

	// lookup an entry and return success or error code
	virtual void lookupEntry(std::string path, IRegistryHandler *handler) = 0;
	
	// insert an entry
	virtual void insert(std::string path, Entry* entry) = 0;

	// load entries from file and add it to the database returning number of entries
	virtual int importFromFile(std::string filename) = 0;
};

#endif // !REGISTRY_H
