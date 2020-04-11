#ifndef COMMAND_GET_H
#define COMMAND_GET_H

#include "Command.h"
#include "IRegistryHandler.h"

class Registry;
class Entry;
class Log;

class CommandGet : public Command, public IRegistryHandler
{
public:
	CommandGet(Registry* registy, Log *log) :
		m_registry(registy),
		m_log(log)
	{}
	~CommandGet() { printf("c_get destructor\n"); }
	// IRequestHandler functions
	void onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response);
	// IRegistryHandler functions
	void onEntryFound(Entry* entry);
	void onEntryFailed(int error);
protected:
	Registry* m_registry;
	Log* m_log;
	std::string m_path;
};

#endif // !COMMAND_GET_H
