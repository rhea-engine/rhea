#include <Rhea/Lobby/LobbyFabric.h>
#include <Rhea/Lobby/ClientConnect.h>
#include <Rhea/Lobby/Avatars.h>
#include <Rhea/Lobby/CredentialsService.h>

namespace Meworla
{
	void LobbyFabric::Register(rhea::Application* app)
	{
		// register types
		simpleparser::Type::registerType<ClientConnect>("//meworla/lobby/connect");
		simpleparser::Type::registerType<Avatars>("//meworla/lobby/avatars");
		simpleparser::Type::registerType<CredentialsService>("//meworla/lobby/credentials");
	}
}
