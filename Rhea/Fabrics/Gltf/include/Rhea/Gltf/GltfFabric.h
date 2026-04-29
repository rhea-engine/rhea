/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GLTFFABRIC_H
#define RHEA_GLTFFABRIC_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea
{	
	/** Fabrics for gltf file loading
	*/
	class GltfFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Gltf"; }
	};
}

#endif // !RHEA_GLTF_H
