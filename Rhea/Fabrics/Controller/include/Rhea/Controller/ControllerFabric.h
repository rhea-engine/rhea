/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_CONTROLLERFABRICS_H
#define RHEA_CONTROLLERFABRICS_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea
{
	/** Fabric for controllers
	*/
	class ControllerFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Controller"; }
	};
}

#endif // !RHEA_CONTROLLERFABRICS_H
