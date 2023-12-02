#ifndef FORTYTWO_AUTHORIZATION_H
#define FORTYTWO_AUTHORIZATION_H

#include <string>
#include <vector>

// Base class for Authorization Handlers

class Authorization
{
public:

	virtual std::string getCode() = 0;

	virtual std::string getUserID(std::string username, std::vector<std::string> secrets) = 0;

	virtual ~Authorization() {}
};

#endif // !FORTYTWO_AUTHORIZATION_H
