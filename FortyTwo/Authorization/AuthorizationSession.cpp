/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "AuthorizationSession.h"
#include "AuthBackend.h"

std::string AuthorizationSession::getCode()
{
	return std::string();
}

void AuthorizationSession::getUserID(std::string username, std::vector<std::string> secrets, LookupHandler handler)
{
	handler(AuthInfo());
}

void AuthorizationSession::SessionToken(AuthInfo& info, LookupHandler handler)
{
	info.session_token = "a token";
	m_backend->SetSessionKey(info.user_id, info.session_token, [&, info, handler](int err) {
		handler(info);
		});
}
