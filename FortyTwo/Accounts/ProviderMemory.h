#ifndef FORTYTWO_PROVIDER_MEMORY_H
#define FORTYTWO_PROVIDER_MEMORY_H

#include "Provider.h"

#include <unordered_map>

class ProviderMemory : public Provider
{
public:
	void Setup(Rhea::Application* app, std::unordered_map<std::string, Variant>& values) {}

	std::vector<Rhea::Unit::Export> getReferences() {
		return std::vector<Rhea::Unit::Export>();
	}

	User* getUser(std::string user_name);

	void addUser(User* user);

protected:
	std::unordered_map<std::string, User*> m_users;
};

#endif // !FORTYTWO_PROVIDER_MEMORY_H
