#include "AuthorizationTrust.h"

std::string AuthorizationTrust::getCode()
{
	return std::string();
}

std::string AuthorizationTrust::getUserID(std::string username, std::vector<std::string> secrets)
{
	return username;
}
