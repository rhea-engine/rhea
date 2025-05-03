#ifndef RHEA_CONTROLLER_H

#include <Rhea/Application.h>
#include <Rhea/ModuleSystem.h>

namespace Rhea
{
	/** Fabrics for basic image loading
	*/
	class ControllerFabrics : public Rhea::IFabricModule
	{
	public:
		void Register(Application* app);

		const char* getName() const override { return "Controller"; }
	};
}

#endif // !RHEA_CONTROLLER_H
