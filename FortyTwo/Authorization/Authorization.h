#ifndef FORTYTWO_AUTHORIZATION_H
#define FORTYTWO_AUTHORIZATION_H

#include <string>
#include <vector>
#include <functional>
#include "AuthInfo.h"
#include "AuthBackend.h"

// Base class for Authorization Handlers

class Authorization
{
public:

	void setAuthBackend(AuthBackend* backend) { m_backend = backend; }

	virtual std::string getCode() = 0;
	
	virtual void getUserID(std::string username, std::vector<std::string> secrets, LookupHandler handler) = 0;

	virtual ~Authorization() {}

protected:
	AuthBackend* m_backend = nullptr;
};

#endif // !FORTYTWO_AUTHORIZATION_H
