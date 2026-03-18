#ifndef RHEA_PLUGIN_H
#define RHEA_PLUGIN_H

namespace Rhea
{
	class Plugin
	{
	public:
		virtual void install() = 0;
		virtual void startup() = 0;
		virtual void shutdown() = 0;
		virtual void uninstall() = 0;		
	};
}

#endif // !RHEA_PLUGIN_H
