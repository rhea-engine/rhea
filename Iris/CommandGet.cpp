#include "CommandGet.h"
#include "Hermes/Connection.h"
#include "Registry.h"
#include "Components/Log/Log.h"

void CommandGet::onRequest(std::shared_ptr<Request> request, std::shared_ptr<Response> response)
{
	if (!response)
		return;

	Command::onRequest(request, response);

	// search for entry
	m_path = request->getValue();
	
	m_registry->lookupEntry(m_path, this);	
}

void CommandGet::onEntryFound(Entry* entry)
{
	m_log->info("200 %s", m_path.c_str());
	std::string res = entry->toJSON();
	m_res->setHeader("Content-type", "application/json");
	m_res->send((unsigned char*)res.c_str(), strlen(res.c_str()));
}

void CommandGet::onEntryFailed(int error)
{
	m_log->info("404 %s", m_path.c_str());
	m_res->sendError(404);
}
