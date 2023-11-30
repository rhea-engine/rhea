#ifndef FORTYTWO_ACCOUNTSERVICE_H
#define FORTYTWO_ACCOUNTSERVICE_H

#include <memory>
#include <Rhea/Service.h>
#include "Provider.h"

class User;

class AccountService : public Rhea::Service
{
	MECLASS(AccountService, Rhea::Service)
public:
	AccountService(std::string name, std::string type) :
		m_anonymous(false),
		Rhea::Service(name, type)
	{}

	void setup(Rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);
	// set a provider directly
	void setProvider(std::unique_ptr<Provider> provider);

	// get provider to act on it
	Provider* getProvider() { return m_provider.get(); }

	User* getUser(std::string method, std::string username, std::vector<std::string> secrets);

	// get available methods for authentication
	std::vector<std::string> getMethods() { return m_methods; }

	// setup

	std::vector<Rhea::Unit::Export> getReferences();

	static void RegisterAccountFabrics(Rhea::Application* app);

protected:
	std::unique_ptr<Provider> m_provider;
	std::string m_provider_type;
	bool m_anonymous;
	std::vector<std::string> m_methods;
};

#endif // !FORTYTWO_ACCOUNTSERVICE_H
