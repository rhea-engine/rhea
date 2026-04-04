#include "Rhea/Network/NetworkFabric.h"
#include <SimpleParser/Type.h>
#include "Rhea/Network/ClientService.h"
#include "Rhea/Network/NetworkComponent.h"

void rhea::NetworkFabric::Register(Application* app)
{
	simpleparser::Type::registerType<NetworkComponent>("//meworla/networkcomponent");
	simpleparser::Type::registerType<ClientService>("//meworla/clientservice");
}
