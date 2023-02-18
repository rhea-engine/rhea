#include "CommandGet.h"
#include "Registry.h"
#include <Psio/Log.h>
#include <Psio/Net/Http/HttpHeaders.h>

int CommandGet::OnRequest(Psio::HttpRequest* req, Psio::HttpResponse* res)
{
	// search for entry
	m_path = req->headers.GetUrl();

	int ret = m_registry->lookupEntry(m_path, [&](Entry* entry) -> int {
		if (entry) {
			LOG(LOG_INFO) << "200 " << m_path;
			std::string resstr = entry->toJSON() + "\n";
			res->setHeader("Content-type", "application/json");
			res->setContentLength(resstr.length());
			res->Write(resstr.c_str(), strlen(resstr.c_str()));
			return res->end();
		}
		else {
			LOG(LOG_INFO) << "404 " << m_path;;
			return res->sendError(404);
		}
		});
	return ret;
}
