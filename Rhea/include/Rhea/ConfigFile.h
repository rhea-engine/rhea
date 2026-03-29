#ifndef RHEA_CONFIGFILE_H
#define RHEA_CONFIGFILE_H

#include <Rhea/Unit.h>

namespace Rhea
{
	class ConfigFile : public Unit
	{
		MECLASS(ConfigFile, Unit)
	public:
		ConfigFile(std::string name, std::string type_name) :
			Unit(name, type_name)
		{}
		ConfigFile(std::string name) :
			ConfigFile(name, "//core/types/configfile")
		{}

		bool loadFromFile(std::string filename);

		// setup form command line arguments
		bool fromCommandLineArgs(int argc, char* argv[]);
	};
}

#endif // !RHEA_CONFIGFILE_H
