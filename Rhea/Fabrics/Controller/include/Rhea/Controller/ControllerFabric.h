#ifndef RHEA_CONTROLLERFABRICS_H
#define RHEA_CONTROLLERFABRICS_H

#include <Rhea/Application.h>
#include <Rhea/FabricSystem.h>

namespace rhea
{
	/** Fabric for controllers
	*/
	class ControllerFabric : public rhea::IFabric
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Controller"; }
	};
}

#endif // !RHEA_CONTROLLERFABRICS_H
