#ifndef FORTYTWO_AUHORIZATIONTRUST_H
#define FORTYTWO_AUHORIZATIONTRUST_H

#include "Authorization.h"

class AuthorizationTrust : public Authorization
{
public:

	std::string getCode();

	std::string getUserID(std::string username, std::vector<std::string> secrets);
};

#endif // !FORTYTWO_AUHORIZATIONTRUST_H
