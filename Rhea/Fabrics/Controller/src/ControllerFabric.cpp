#include <Rhea/Controller/ControllerFabric.h>

#include <Rhea/Controller/BasicController.h>
#include <Rhea/Controller/SpectatorController.h>

void rhea::ControllerFabric::Register(Application* app)
{
	SimpleParser::Type::registerType<BasicController>("//meworla/controller");
	SimpleParser::Type::registerType<SpectatorController>("//meworla/spectator");
}
