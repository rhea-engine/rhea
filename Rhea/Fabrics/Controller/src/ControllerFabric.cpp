#include <Rhea/Controller/ControllerFabric.h>

#include <Rhea/Controller/BasicController.h>
#include <Rhea/Controller/SpectatorController.h>

void rhea::ControllerFabric::Register(Application* app)
{
	simpleparser::Type::registerType<BasicController>("//meworla/controller");
	simpleparser::Type::registerType<SpectatorController>("//meworla/spectator");
}
