#ifndef RHEACLIENT_GRAPHICS_LIGHTMANAGER_H
#define RHEACLIENT_GRAPHICS_LIGHTMANAGER_H

#include <RheaClient/Graphics/GraphicsManager.h>
#include <Rhea/Core/Light.h>

namespace rhea::client
{
	class GraphicsLightManager : public GraphicsManager
	{
	public:
		GraphicsLightManager(GraphicsService* service) :
			GraphicsManager(service)
		{
		}
		~GraphicsLightManager();

		// create an element for this unit
		bool Ensure(poseidon::Core* core, rhea::Unit* unit) override;

		// handle update event
		bool onUpdate(rhea::Event* ev, rhea::Unit* unit);

		// handle frame update
		void onFrame(float delta) override {};

		// handle deletion event
		bool onLightDelete(rhea::Event* ev, rhea::Unit* unit);

		void syncLight(poseidon::Core* core, poseidon::LightHandle handle, rhea::Light* source);

	private:
		bool createGPUResources(poseidon::Core* core,
			rhea::Item* item,
			RenderComponent* rc,
			rhea::Mesh* mesh);
	};
}

#endif // !RHEACLIENT_GRAPHICS_LIGHTMANAGER_H
