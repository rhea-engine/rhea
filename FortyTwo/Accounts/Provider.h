#ifndef FORTYTWO_ACCOUNTS_PROVIDER_H
#define FORTYTWO_ACCOUNTS_PROVIDER_H

#include <Rhea/Application.h>

class User;

class Provider
{
public:
	virtual ~Provider() {}

	virtual void Setup(Rhea::Application* app, std::unordered_map<std::string, Variant> &values) = 0;

	virtual std::vector<Rhea::Unit::Export> getReferences() = 0;

	virtual User* getUser(std::string user_name) = 0;
};

#endif // !FORTYTWO_ACCOUNTS_PROVIDER_H
