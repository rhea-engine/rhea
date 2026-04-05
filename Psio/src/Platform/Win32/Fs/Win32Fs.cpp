#include <Psio/Fs/Fs.h>

#include <windows.h>
#include <initguid.h>
#include <KnownFolders.h>
#include <shlobj.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <Psio/Log.h>
#include <tchar.h> 
#include <strsafe.h>

using std::string;

namespace psio
{
    static bool widePathToString(string& dest, const WCHAR* wpath)
    {
        UINT codepage = CP_ACP;
        const int wlength = static_cast<int>(wcslen(wpath));
        const int length = WideCharToMultiByte(codepage, 0, wpath, wlength, NULL, 0, NULL, NULL);
        if (length <= 0)
        {
            dest.clear();
            return false;
        }

        // success
        dest.resize(length);
        WideCharToMultiByte(codepage, 0, wpath, wlength, &dest[0], (int)dest.size(), NULL, NULL);
        return true;
    }

    string Fs::getAppDataDir()
    {
        PWSTR path = NULL;

        if (!m_app_dir.empty())
            return m_app_dir;

        HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path);

        string res;
        widePathToString(res, path);

        CoTaskMemFree(path);
        
        res += string("/") + m_appname;

        m_app_dir = res;

        if (!fileExists(m_app_dir)) {
            int check = _mkdir(m_app_dir.c_str());
            if (check) {
                LOG(LOG_ERROR) << "could not create app data folder: " << errno;
            }
        }

        return res;
    }

	void Fs::setAppDataDir(std::string app_dir)
	{
        m_app_dir = app_dir;
	}

    string Fs::getInstallDataDir()
    {
        static string res;

        // if alredy found return it
        if (res.length())
            return res;

        // try local
        res = string("Data");
        if (Fs::fileExists(res))
            return res;

        // nope - try program files
        PWSTR path = NULL;

        HRESULT hr = SHGetKnownFolderPath(FOLDERID_ProgramFiles, 0, NULL, &path);

        widePathToString(res, path);

        CoTaskMemFree(path);

        res += string("/") + m_appname + "/Data";
        if (Fs::fileExists(res))
            return res;

        // try to get path to executable
		char buf[MAX_PATH];
        if (GetModuleFileNameA(NULL, buf, MAX_PATH) > 0) {
			// store buf in res
			res = buf;
			res = res.substr(0, res.find_last_of("\\/"));
			res += string("/Data");
			if (Fs::fileExists(res))
				return res;
        }

        return "";
    }

    std::string Fs::getHomeDir(bool create)
    {
        if (m_home.length())
            return m_home;

        char* home_env = getenv("HOMEPATH"); 
        if (!home_env)
            home_env = getenv("APPDATA");
        if (home_env)
            m_home = std::string(home_env);
        else
            m_home = ".";

        m_home += "\\Meworla";
        if (!fileExists(m_home) && create) {
            int check = _mkdir(m_home.c_str());
            if (check) {
                LOG(LOG_ERROR) << "could not create data folder in users home: " << errno;
            }
        }

        return m_home;

    }

    int Fs::fileOpen(FILE** f, string filename, int mode)
    {
        char* accessMode = "";

        if (mode == Fs::READ)
            accessMode = "r";
        else if (mode == Fs::WRITE)
            accessMode = "w";
        else if (mode == (Fs::READ | Fs::WRITE))
            accessMode = "r+";
        else if (mode == (Fs::READ | Fs::BINARY))
            accessMode = "rb";
        else if (mode == (Fs::WRITE | Fs::BINARY))
            accessMode = "wb";
        else if (mode == (Fs::READ | Fs::WRITE | Fs::BINARY))
            accessMode = "rb+";
        else if (mode == Fs::APPEND)
            accessMode = "a+";
        else if (mode == (Fs::APPEND | Fs::BINARY))
            accessMode = "ab+";

        else {
            printf("Fs::fileOpen: Unknown mode\n");
        }

        errno_t errcode = fopen_s(f, filename.c_str(), accessMode);
        if (errcode) {
            return errcode;
        }
        else
            return 0;
    }

    bool Fs::fileExists(const string& path)
    {
        return _access(path.c_str(), 04) == 0;
    }

    bool Fs::createDirectory(const std::string& path)
    {
        int check = _mkdir(path.c_str());
        if (check) {
            LOG(LOG_ERROR) << "could not create folder: " << errno;
            return false;
        }
        return true;
    }

    class WinDirectoryHandle : public Fs::DirectoryHandle
    {
    public:
        ~WinDirectoryHandle() {
            FindClose(hFind);
        }
        
        WIN32_FIND_DATA ffd;
        HANDLE hFind = INVALID_HANDLE_VALUE;
    };

    Fs::DirectoryHandle* Fs::openDirectory(std::string path)
    {        
        WinDirectoryHandle *fh = new WinDirectoryHandle();
        path += "/*.*";

        fh->hFind = FindFirstFileA(path.c_str(), &fh->ffd);
        if (fh->hFind == INVALID_HANDLE_VALUE) {
            delete fh;
            return nullptr;
        }
        else
            return fh;
    }

    int Fs::readEntry(DirectoryHandle* handle, FileAttributes* attributes)
    {
        WinDirectoryHandle* fh = static_cast<WinDirectoryHandle*>(handle);
        
        attributes->name = fh->ffd.cFileName;
        WIN32_FIND_DATA ffd = fh->ffd;
        if (fh->ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            attributes->is_dir = true;
            attributes->size = 0;
        }
        else {
            attributes->is_dir = false;
            uint64_t size = (fh->ffd.nFileSizeHigh * (static_cast<uint64_t>(MAXDWORD) + 1)) + fh->ffd.nFileSizeLow;
            attributes->size = size;
        }
        return 0;
    }

    bool Fs::nextEntry(DirectoryHandle* handle) 
    {
        WinDirectoryHandle* fh = static_cast<WinDirectoryHandle*>(handle);
        return FindNextFileA(fh->hFind, &fh->ffd);
    }

}
