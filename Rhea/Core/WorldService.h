#ifndef RHEA_WORLDSERVICE_H
#define RHEA_WORLDSERVICE_H

#include <Rhea/Service.h>
#include "Scene.h"

namespace Rhea
{
	/** The main service that manages scene and thei lifecycles
	*/
	class WorldService : public Service
	{
		MECLASS(WorldService, Service)
	public:
		WorldService::WorldService(std::string name, std::string type_name) :
			Service(name, type_name)
		{}

		// init event
		virtual void onInit();

		// every frame
		bool onFrame(float delta) override;
		bool onFrameFinal(float delta) override;

	protected:
		// prepare a scene to display
		void onSceneActivate(Scene* scene);
		void onSceneDeactivate(Scene* scene);

		// list of active scenes
		std::vector<Scene*> m_scenes;
	};
}
#endif // !RHEA_WORLDSERVICE_H
