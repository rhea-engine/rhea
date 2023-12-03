#ifndef FORTYTWO_AUHORIZATIONTRUST_H
#define FORTYTWO_AUHORIZATIONTRUST_H

#include "Authorization.h"

class AuthorizationTrust : public Authorization
{
public:
	std::string getCode();

	void getUserID(std::string username, std::vector<std::string> secrets, LookupHandler handler);
};

#endif // !FORTYTWO_AUHORIZATIONTRUST_H
