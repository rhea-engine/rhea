#ifndef RHEA_CONTROLLERFABRICS_H
#define RHEA_CONTROLLERFABRICS_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Rhea
{
	/** Fabrics for controllers
	*/
	class ControllerFabrics : public Rhea::IFabricModule
	{
	public:
		void Register(Application* app) override;

		const char* getName() const override { return "Controller"; }
	};
}

#endif // !RHEA_CONTROLLERFABRICS_H
