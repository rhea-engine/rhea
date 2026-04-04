#include <Rhea/Lobby/CredentialsService.h>
#include <Rhea/Application.h>
#include <Orion/FileStream.h>
#include <nlohmann/json.hpp>

namespace Meworla
{
	CredentialsService::Credential* CredentialsService::getCredential(std::string world_name, std::vector<std::string> methods)
	{
		// read from userprefs
		std::string cred_path = m_app->getFS()->getFs()->getAppDataDir();
		cred_path += string("/credential.json");

		if (m_app->getFS()->getFs()->fileExists(cred_path))
		{
			// file exists		
			std::unique_ptr<orion::FileResourceStream> stream = std::make_unique<orion::FileResourceStream>(cred_path);
			std::unique_ptr<psio::IOBuffer> buf = std::make_unique<psio::IOBuffer>(64 * 1024);
			if (stream->Read(buf.get())) {
				const char* ptr = buf->reader();
				size_t len = buf->Remaining();
				nlohmann::json prefs = nlohmann::json::parse(ptr, ptr + len, nullptr, false);

				// check if the world exists
				if (prefs.contains(world_name)) {
					nlohmann::json cred = prefs[world_name];
					// check if the given method exists
					for (auto& method : methods) {
						if (cred.contains(method)) {
							nlohmann::json cred_method = cred[method];
							if (cred_method.contains("username") && cred_method.contains("secrets")) {
								Credential* c = new Credential();
								c->method = method;
								c->username = cred_method["username"].get<std::string>();
								c->secrets = cred_method["secrets"].get<std::vector<std::string>>();
								return c;
							}
						}
					}
				}
			}
		}

		return nullptr;
	}
}