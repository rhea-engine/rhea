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
		Poseidon::Shader* EnsurePoseidonShader(Poseidon::Core* core, rhea::Material* material_res, rhea::Shader* shader_res);

		// create an element for this unit
		bool Ensure(Poseidon::Core* core, rhea::Unit* unit);

		bool FillPoseidonMaterial(Poseidon::Core* core, rhea::Material* material_res, Poseidon::Material* out_mat, bool fresh_build);

		// handle frame update
		void onFrame() {}

		// handle events
		bool onItemDelete(rhea::Event* ev, rhea::Unit* unit);	
	};
}

#endif // !RHEA_GRAPHICS_MATERIAL_MGR_H
