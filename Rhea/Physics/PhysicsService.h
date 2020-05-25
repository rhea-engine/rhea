#ifndef RHEA_PHYSICS_SERVICE_H
#define RHEA_PHYSICS_SERVICE_H

#include "../Service.h"
#include "../Scene.h"

namespace Rhea
{
	class Controller;
	class Item;

	class PhysicsService : public Service
	{
	public:
		PhysicsService(Application* app);
		PhysicsService(Application* app, std::string service_name);

		// process physics with delta seconds (usually 1/60)
		virtual void step(float delta);

		// set scene and initialize objects in it
		virtual void setScene(Scene* scene);

		// Service functions
		void onInit();	

	protected:
		Item* m_item;
	};
}

#endif // !RHEA_PHYSICS_SERVICE_H
