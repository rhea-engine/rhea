/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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
