#ifndef RHEA_NETWORK_FABRICS_H
#define RHEA_NETWORK_FABRICS_H

namespace Rhea
{
	class Application;

	/** Fabrics for network package
	*/
	class NetworkFabrics
	{
	public:
		static void Register(Application* app);
	};
}

#endif // !RHEA_NETWORK_FABRICS_H
