#ifndef REGISTRY_MEMORY_H
#define REGISTRY_MEMORY_H

#include <unordered_map>

#include "Registry.h"

class RegistryMemory : public Registry
{
public:
	// lookup an entry and return success or error code
	int lookupEntry(std::string path, EntryHandler handler);
	// add/replace an entry
	void insert(std::string path, Entry* entry);
	
	// load entries from file and add it to the database
	int importFromFile(std::string filename);
	
	// set the database file name
	void setRegistryFile(std::string filename);

	// load from database
	int load();
protected:
	std::unordered_map<std::string, Entry*> m_entries;
	std::string m_registry_filename;
};

#endif // !REGISTRY_MEMORY_H
