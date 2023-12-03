#include "AuthorizationTrust.h"
#include "AuthBackend.h"

std::string AuthorizationTrust::getCode()
{
	return std::string();
}

void AuthorizationTrust::getUserID(std::string username, std::vector<std::string> secrets, LookupHandler handler)
{
	m_backend->LookupByUsername(username, [&, handler](UserInfo info) {
		AuthInfo auth_info;
		auth_info.name = info.name;
		auth_info.user_id = info.user_id;
		auth_info.provider = "trust";

		handler(auth_info);

		});
}
