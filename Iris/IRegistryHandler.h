#ifndef IREGISTRYHANDLER_H
#define IREGISTRYHANDLER_H

class Entry;

class IRegistryHandler
{
public:
	virtual void onEntryFound(Entry* entry) = 0;
	virtual void onEntryFailed(int error) = 0;
};

#endif // !IREGISTRYHANDLER_H
