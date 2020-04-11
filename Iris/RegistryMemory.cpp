#include <cstdio>
#include <cerrno>
#include <memory>

#include "RegistryMemory.h"
#include "RegistryFileParser.h"

void RegistryMemory::lookupEntry(std::string path, IRegistryHandler* handler)
{
    // lookup in entries array
    auto search = m_entries.find(path);
    // return error if not found
    if (search == m_entries.end())
        return handler->onEntryFailed(404);

    // found
    Entry* entry = search->second;
    handler->onEntryFound(entry);
}

void RegistryMemory::insert(std::string path, Entry* entry)
{
    Entry *e = new Entry(*entry);
    m_entries[path] = e; 
}

void RegistryMemory::setRegistryFile(std::string filename)
{
    m_registry_filename = filename;
}

int RegistryMemory::load()
{
    // load the file using the import function
    return importFromFile(m_registry_filename);
}

#define BUFLEN 8192

int RegistryMemory::importFromFile(std::string filename)
{
    char* data = NULL, * temp;
    size_t buflen, read, size = 0;
    FILE* f;
    std::unique_ptr<RegistryFileParser> parser = std::make_unique<RegistryFileParser>(this);
    
    f = fopen(filename.c_str(), "r");
    if (f == NULL) {
        printf("Could not open registry file: %s\n", strerror(errno));
        return 0;
    }

    data = (char *)malloc(BUFLEN);
    if (!data) {
        printf("Could not allocate memory in RegistryMemory::loadFromFile\n");
        return 0;
    }
    buflen = BUFLEN;

    while (true) {
        read = fread(data + size, 1, buflen - size, f);
        if (read == 0)
            break;
        else if (read < 0) {
            printf("Could not read from registry file: %s\n", strerror(errno));
            free(data);
            return 0;
        }
        size += read;

        // if we read buflen bytes the file is probably not read in full
        // enlarge buffer
        if (read == BUFLEN || buflen - size < 1024) {
            buflen += BUFLEN;
            temp = (char*)realloc(data, buflen);
            if (temp == NULL) {
                printf("Could not allocate memory in RegistryMemory::loadFromFile\n");
                free(data);
                return 0;
            }
            data = temp;
        }
    }

    int entries = parser->parse(data, size);
    free(data);
    return entries;
}
