#ifndef RHEA_RHEAPHYSICSFABRICBULLET_H
#define RHEA_RHEAPHYSICSFABRICBULLET_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea::physics::bullet
{
	class RheaPhysicsBulletFabric : public Rhea::IFabric
	{
	public:
		void Register(Rhea::Application* app) override;

		const char* getName() const override { return "RheaPhysicsBullet"; }
	};
}

#endif // !RHEA_RHEAPHYSICSBULLET_H
