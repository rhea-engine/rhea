/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef FORTYTWO_AUTHORIZATIONSESSION_H
#define FORTYTWO_AUTHORIZATIONSESSION_H

#include "Authorization.h"

class AuthorizationSession : public Authorization
{
public:
	std::string getCode();

	void getUserID(std::string username, std::vector<std::string> secrets, LookupHandler handler);

	void SessionToken(AuthInfo& info, LookupHandler handler);
};


#endif // !FORTYTWO_AUTHORIZATIONSESSION_H
