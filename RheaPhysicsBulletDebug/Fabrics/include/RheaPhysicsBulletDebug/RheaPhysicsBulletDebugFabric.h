#ifndef RHEA_RHEAPHYSICSFABRICBULLETDEBUG_H
#define RHEA_RHEAPHYSICSFABRICBULLETDEBUG_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace Rhea
{
	class RheaPhysicsBulletDebugFabric : public Rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "RheaPhysicsBulletDebug"; }
	};
}

#endif // !RHEA_RHEAPHYSICSBULLETDEBUG_H
