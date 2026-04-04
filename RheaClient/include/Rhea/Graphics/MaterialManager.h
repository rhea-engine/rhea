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
		Poseidon::Shader* EnsurePoseidonShader(Poseidon::Core* core, Rhea::Material* material_res, Rhea::Shader* shader_res);

		// create an element for this unit
		bool Ensure(Poseidon::Core* core, Rhea::Unit* unit);

		bool FillPoseidonMaterial(Poseidon::Core* core, Rhea::Material* material_res, Poseidon::Material* out_mat, bool fresh_build);

		// handle frame update
		void onFrame() {}

		// handle events
		bool onItemDelete(Rhea::Event* ev, Rhea::Unit* unit);	
	};
}

#endif // !RHEA_GRAPHICS_MATERIAL_MGR_H
