#ifndef ORION_OS_FS_H
#define ORION_OS_FS_H

#include <string>
#include <stdio.h>

using std::string;

namespace Orion
{
	class Fs
	{
	protected:
		string m_appname;
	public:
		Fs(string appname);

		// get the app data dir 
		// returns a directory path name depending on the OS
		// the function does not check if the path exists
		string getAppDataDir();

		// get the dir of the installed data
		// this is the root of all system and addon realms
		string getInstallDataDir();

		// open the file using fopen
		static int fileOpen(FILE** f, string filename, int mode);

		// Test if the given file exists
		static bool fileExists(const string& path);

		static const int READ = 1;
		static const int WRITE = 2;
	};
}

#endif // !ORION_OS_FS_H
