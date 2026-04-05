#include "AuthorizationService.h"
#include "AuthorizationTrust.h"
#include "AuthorizationSession.h"
#include <Rhea/Application.h>
#include <Psio/Log.h>
#include "AuthBackendFile.h"

void AuthorizationService::setup(rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
{
	rhea::Service::setup(app, attributes, values);
	m_app->registerService("authorization", this);

	// create auth backend
	m_backend = std::make_unique<AuthBackendFile>();
	m_backend->setup(app);

	if (m_values.find("methods") != m_values.end()) {
		Variant methods = m_values["methods"];
		for (int i = 0; i < methods.size(); i++) {
			std::string method = (std::string)methods[i];
			if (method == "trust") {
				m_authorizations[method] = new AuthorizationTrust();
			}
			else if (method=="session") {
				m_authorizations[method] = new AuthorizationSession();
				m_sessions_enabled = true;
			}
			else {
				LOG(LOG_DEBUG) << "Not implemented authorization method '" << method << "' defined";
				continue;
			}
			m_authorizations[method]->setAuthBackend(m_backend.get());
			m_methods.push_back((std::string)methods[i]);
		}
	}
	if (m_values.find("anonymous") != m_values.end()) {
		m_anonymous = m_values["anonymous"];
		if (m_anonymous) {
			m_methods.push_back("anonymous");
		}
	}
	
}

void AuthorizationService::getUserId(std::string method, std::string username, std::vector<std::string> secrets, LookupHandler handler)
{
	if (m_authorizations.find(method) != m_authorizations.end())
	{
		if (!m_sessions_enabled) {
			m_authorizations[method]->getUserID(username, secrets, handler);
		}
		else {
			m_authorizations[method]->getUserID(username, secrets, [&, handler](AuthInfo info) {
				if (info.user_id == "") {
					handler(info);
				}
				else {
					static_cast<AuthorizationSession*>(m_authorizations["session"])->SessionToken(info, handler);
				}
				});
		}
	}
	else
	{
		AuthInfo info;
		handler(info);
	}	
}

void AuthorizationService::CreateUser(std::string method, UserInfo info, std::vector<std::string> secrets, std::function<void(std::string, int err)> handler)
{
	// safety check
	if (info.username == "" || info.username == "anonymous") {
		handler("", -2);
		return;
	}
	// does username already exist
	info.user_id = info.username;
	m_backend->CreateUser(info, secrets, handler);
}

void AuthorizationService::RegisterFabrics(rhea::Application* app)
{
	// register types
	simpleparser::Type::registerType<AuthorizationService>("//meworla/authorizationservice");
}
