#ifndef MEWORLA_LOBBY_CREDENTIRALSSERVICE_H
#define MEWORLA_LOBBY_CREDENTIRALSSERVICE_H

#include <Rhea/Service.h>

namespace Meworla
{
	class CredentialsService : public Rhea::Service
	{
		MECLASS(CredentialsService, Rhea::Service)
	public:
		CredentialsService(std::string name, std::string type) :
			Rhea::Service(name, type)
		{}

		struct Credential
		{
			std::string method;
			std::string username;
			std::vector<std::string> secrets;
		};

		Credential* getCredential(std::string world_name, std::vector<std::string> methods);

	protected:

	};
}

#endif // !MEWORLA_LOBBY_CREDENTIRALSSERVICE_H
