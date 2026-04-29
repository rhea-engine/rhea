/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/Network/NetworkFabric.h"
#include <SimpleParser/Type.h>
#include "Rhea/Network/ClientService.h"
#include "Rhea/Network/NetworkComponent.h"

void rhea::NetworkFabric::Register(Application* app)
{
	simpleparser::Type::registerType<NetworkComponent>("//meworla/networkcomponent");
	simpleparser::Type::registerType<ClientService>("//meworla/clientservice");
}
