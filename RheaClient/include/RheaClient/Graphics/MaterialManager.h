/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef RHEA_GRAPHICS_MATERIAL_MGR_H
#define RHEA_GRAPHICS_MATERIAL_MGR_H

#include <vector>
#include "GraphicsManager.h"
#include <Poseidon/Shader.h>
#include <Rhea/Core/Shader.h>

namespace rhea::client {
	class GraphicsMaterialManager : public GraphicsManager
	{
	public:
		GraphicsMaterialManager(GraphicsService* service) :
			GraphicsManager(service)
		{}

		// ensure shader is created
		poseidon::Shader* EnsurePoseidonShader(poseidon::Core* core, rhea::Material* material_res, rhea::Shader* shader_res);

		// create an element for this unit
		bool Ensure(poseidon::Core* core, rhea::Unit* unit);

		bool FillPoseidonMaterial(poseidon::Core* core, rhea::Material* material_res, poseidon::Material* out_mat, bool fresh_build);

		// handle frame update
		void onFrame(float delta) override {}

		// handle events
		bool onItemDelete(rhea::Event* ev, rhea::Unit* unit);	
	};
}

#endif // !RHEA_GRAPHICS_MATERIAL_MGR_H
