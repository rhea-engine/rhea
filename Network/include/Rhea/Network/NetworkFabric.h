#ifndef RHEA_NETWORK_FABRICS_H
#define RHEA_NETWORK_FABRICS_H

#include <Rhea/ModuleSystem.h>

namespace Rhea
{
	class Application;

	/** Fabrics for network package
	*/
	class NetworkFabric : public Rhea::IFabric
	{
	public:
		void Register(Application* app) override;
		const char* getName() const override { return "RheaNetwork"; }
	};
}

#endif // !RHEA_NETWORK_FABRICS_H
