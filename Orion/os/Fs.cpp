#include "Fs.h"
#ifdef WIN32
#include <direct.h>
#endif
#include <Psio/Log.h>

namespace Orion
{
	Fs::Fs(std::string appname)
	{
		m_appname = appname;
	}

	std::string Fs::getHomeDir()
	{
        if (m_home.length())
            return m_home;

#ifdef WIN32
        char* home_env = getenv("APPDATA");       
		if (!home_env)
			home_env = getenv("HOMEPATH");
		if (home_env)
			m_home = std::string(home_env);
		else
			m_home = ".";

		m_home += "\\Meworla";
		if (!fileExists(m_home)) {
			int check = _mkdir(m_home.c_str());
			if (check) {
				LOG(LOG_ERROR) << "could not create appdata folder: " << errno;
			}
		}

#endif

return m_home;

	}
}