/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef FORTYTWO_AUTHINFO_H
#define FORTYTWO_AUTHINFO_H

#include <string>
#include <functional>

struct AuthInfo
{
	std::string user_id;
	std::string name;
	std::string session_token;
	std::string provider;
	std::string token;
};

typedef std::function<void(AuthInfo)> LookupHandler;

#endif // !FORTYTWO_AUTHINFO_H
