#include "AuthorizationService.h"
#include "AuthorizationTrust.h"
#include <Rhea/Application.h>

void AuthorizationService::setup(Rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
{
	Rhea::Service::setup(app, attributes, values);
	m_authorizations["trust"] = new AuthorizationTrust();
	m_app->registerService("authorization", this);

	if (m_values.find("methods") != m_values.end()) {
		Variant methods = m_values["methods"];
		for (int i = 0; i < methods.size(); i++) {
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

std::string AuthorizationService::getUserId(std::string method, std::string username, std::vector<std::string> secrets)
{
	if (m_authorizations.find(method) != m_authorizations.end())
	{
		return m_authorizations[method]->getUserID(username, secrets);
	}
	else
	{
		return "";
	}	
}

void AuthorizationService::RegisterFabrics(Rhea::Application* app)
{
	// register types
	SimpleParser::Type::registerType<AuthorizationService>("//meworla/authorizationservice");
}
