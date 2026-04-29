/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_OBJ_FABRIC_H
#define RHEA_OBJ_FABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea
{
	class Application;

	/** Fabrics for basic image loading
	*/
	class ObjFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;
		const char* getName() const override { return "Obj"; }
	};
}

#endif // !RHEA_OBJ_FABRIC_H
