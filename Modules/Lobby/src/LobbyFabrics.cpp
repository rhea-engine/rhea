#include <Rhea/Lobby/LobbyFabrics.h>
#include <Rhea/Lobby/ClientConnect.h>
#include <Rhea/Lobby/Avatars.h>
#include <Rhea/Lobby/CredentialsService.h>

namespace Meworla
{
	void LobbyFabrics::Register(Rhea::Application* app)
	{
		// register types
		SimpleParser::Type::registerType<ClientConnect>("//meworla/lobby/connect");
		SimpleParser::Type::registerType<Avatars>("//meworla/lobby/avatars");
		SimpleParser::Type::registerType<CredentialsService>("//meworla/lobby/credentials");
	}
}
