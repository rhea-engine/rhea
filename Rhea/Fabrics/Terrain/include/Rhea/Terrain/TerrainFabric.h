/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_TERRAINFABRIC_H
#define RHEA_TERRAINFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea
{
	/** Fabrics for terrain handling
	*/
	class TerrainFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Terrain"; }
	};
}

#endif // !RHEA_TERRAINFABRIC_H
