#include "NetworkFabrics.h"
#include <SimpleParser/Type.h>
#include <ClientService.h>
#include <NetworkComponent.h>

void Rhea::NetworkFabrics::Register(Application* app)
{
	SimpleParser::Type::registerType<NetworkComponent>("//meworla/networkcomponent");
	SimpleParser::Type::registerType<ClientService>("//meworla/clientservice");
}
