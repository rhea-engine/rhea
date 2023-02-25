#ifndef ORION_OS_FS_H
#define ORION_OS_FS_H

#include <string>
#include <stdio.h>

namespace Orion
{
	class Fs
	{
	protected:
		std::string m_appname;
		std::string m_app_dir;
		std::string m_home;
	public:
		Fs(std::string appname);

		// get the app data dir 
		// returns a directory path name depending on the OS
		// the function does not check if the path exists
		std::string getAppDataDir();

		// set the app data dir manually
		void setAppDataDir(std::string app_dir);

		// get the dir of the installed data
		// this is the root of all system and addon realms
		std::string getInstallDataDir();

		// open the file using fopen
		static int fileOpen(FILE** f, std::string filename, int mode);

		// Test if the given file exists
		static bool fileExists(const std::string& path);

		// get user home dir
		std::string getHomeDir();

		static const int READ = 1;
		static const int WRITE = 2;
		static const int BINARY = 4;
	};
}

#endif // !ORION_OS_FS_H
