#ifndef FORTYTWO_AUTHBACKEND_H
#define FORTYTWO_AUTHBACKEND_H

#include <functional>
#include <string>
#include "AuthInfo.h"
#include <Rhea/Application.h>

struct UserInfo {
	std::string user_id;
	std::string username;
	std::string ext_id;
	std::string name;
	std::string session_key;
	std::string password_hash;
};

class AuthBackend
{
public:

	virtual void setup(rhea::Application* app) = 0;

	typedef std::function<void(UserInfo info)> DBLookupHandler;

	virtual void LookupByUserID(std::string user_id, DBLookupHandler handler) = 0;
	virtual void LookupByUsername(std::string user_id, DBLookupHandler handler) = 0;
	virtual void LookupByExternalID(std::string ext_id, DBLookupHandler handler) = 0;
	virtual void LookupBySessionKey(std::string session_id, DBLookupHandler handler) = 0;

	virtual void SetSessionKey(std::string user_id, std::string session_id, std::function<void(int err)> handler) = 0;

	virtual void CreateUser(UserInfo info, std::vector<std::string> secrets, std::function<void(std::string, int)> handler) = 0;
};

#endif // !FORTYTWO_AUTHBACKEND_H
