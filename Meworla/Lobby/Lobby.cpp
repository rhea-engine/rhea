#include "Lobby.h"
#include "ClientConnect.h"
#include "Avatars.h"

namespace Meworla
{
	void Lobby::ready()
	{
		Rhea::Unit::ready();
	}

	void Lobby::registerFabrics(Rhea::Application* app)
	{
		// register types
		SimpleParser::Type::registerType<Lobby>("//meworla/lobby");
		SimpleParser::Type::registerType<ClientConnect>("//meworla/lobby/connect");
		SimpleParser::Type::registerType<Avatars>("//meworla/lobby/avatars");
	}
}
