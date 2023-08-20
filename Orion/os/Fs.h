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
		Fs(std::string appname) : 
			m_appname(appname)
		{}

		// get the app data dir 
		// this is the directory where we store user preferences
		// cache etc.
		// returns a directory path name depending on the OS
		// the function does not check if the path exists
		std::string getAppDataDir();

		// set the app data dir manually
		void setAppDataDir(std::string app_dir);

		// get users home directory
		// this is used if a user create its own projects
		std::string getHomeDir(bool create = false);

		// set the home dir manually
		void setHomeDir(std::string home_dir);

		// get the dir of the installed data
		// this is the root of all system and addon realms
		std::string getInstallDataDir();

		// open the file using fopen
		static int fileOpen(FILE** f, std::string filename, int mode);

		// Test if the given file exists
		static bool fileExists(const std::string& path);

		static const int READ = 1;
		static const int WRITE = 2;
		static const int BINARY = 4;

		// directory functions
		struct FileAttributes
		{
			std::string name;
			size_t size;
			bool is_dir;

		};

		class DirectoryHandle {
		public:
			virtual ~DirectoryHandle() {}
		};
		static DirectoryHandle* openDirectory(std::string path);
		static int readEntry(DirectoryHandle* handle, FileAttributes* attributes);
		static bool nextEntry(DirectoryHandle* handle);

	};
}

#endif // !ORION_OS_FS_H
