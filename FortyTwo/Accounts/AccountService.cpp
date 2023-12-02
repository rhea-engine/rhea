#include "AccountService.h"
#include "User.h"
#include "ProviderScene.h"
#include "ProviderMemory.h"
#include <Psio/Log.h>

void AccountService::setup(Rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values)
{
	Unit::setup(app, attributes, values);

	// Sample config:
	// [accounts]
	// type = "//meworla/accountservice"
	// provider = "scene"
	// avatarlist = "//local/avatarlist.scene"

	// set up the provider if one is given
	if (m_values.find("provider") != m_values.end()) {
		m_provider_type = m_values["provider"];
		if (m_provider_type == "scene") {
			m_provider = std::make_unique<ProviderScene>();
		}
		else if (m_provider_type == "memory") {
			m_provider = std::make_unique<ProviderMemory>();
		}
		if (m_provider) {
			m_provider->Setup(app, m_values);
		}
	}	
}

void AccountService::setProvider(std::unique_ptr<Provider> provider)
{
	m_provider = std::move(provider);
}

User* AccountService::getUser(std::string user_id)
{
	if (m_provider)
		return m_provider->getUser(user_id);
	else
		return nullptr;
}

std::vector<Rhea::Unit::Export> AccountService::getReferences()
{
	std::vector<Unit::Export> refs;
	if (m_provider)
		refs = m_provider->getReferences();
	return refs;
}

void AccountService::RegisterFabrics(Rhea::Application* app)
{
	// register types
	SimpleParser::Type::registerType<User>("//meworla/user");
	SimpleParser::Type::registerType<AccountService>("//meworla/accountservice");
}
