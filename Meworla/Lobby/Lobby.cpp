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

	Rhea::Unit* Lobby::createType(std::string name, std::string type)
	{
		Rhea::Unit* ret = (Unit*)new Lobby(name, type);
		return ret;
	}
}
