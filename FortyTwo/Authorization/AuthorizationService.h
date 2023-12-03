#ifndef FORTYTWO_AUTHORIZATIONSERVICE_H
#define FORTYTWO_AUTHORIZATIONSERVICE_H

#include <Rhea/Service.h>
#include "Authorization.h"
#include "AuthBackend.h"
#include "AuthInfo.h"

class AuthorizationService : public Rhea::Service
{
	MECLASS(AuthorizationService, Rhea::Service)
public:
	AuthorizationService(std::string name, std::string type) :
		m_anonymous(false),
		m_sessions_enabled(false),
		Rhea::Service(name, type)
	{}
	virtual ~AuthorizationService() {}

	void setup(Rhea::Application* app, std::unordered_map<std::string, std::string> attributes, std::unordered_map<std::string, Variant> values);

	// get the challenge for oauth2
	void getChallenge() {}

	// try to login with the given credentials
	// in oauth2 this is the second step
	void getUserId(std::string method, std::string username, std::vector<std::string> secrets, LookupHandler handler);

	void CreateUser(std::string method, UserInfo info, std::vector<std::string> secrets, std::function<void(std::string, int err)> handler);

	// get available methods for authentication
	std::vector<std::string> getMethods() { return m_methods; }

	static void RegisterFabrics(Rhea::Application* app);

protected:
	std::unordered_map<std::string, Authorization*> m_authorizations;

	std::vector<std::string> m_methods;
	bool m_anonymous;
	bool m_sessions_enabled;
	std::unique_ptr<AuthBackend> m_backend;
};

#endif // !FORTYTWO_AUTHORIZATIONSERVICE_H
