#include "Fs.h"

#include <windows.h>
#include <initguid.h>
#include <KnownFolders.h>
#include <shlobj.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>

namespace Orion
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

        HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &path);

        string res;
        widePathToString(res, path);

        CoTaskMemFree(path);
        
        res += string("/") + m_appname;

        return res;
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

        return "";
    }

    int Fs::fileOpen(FILE** f, string filename, int mode)
    {
        char* accessMode = "";

        if (mode == Fs::READ)
            accessMode = "r";
        if (mode == Fs::WRITE)
            accessMode = "w";
        if (mode == (Fs::READ | Fs::WRITE))
            accessMode = "r+";

        errno_t errcode = fopen_s(f, filename.c_str(), accessMode);
        if (f == NULL) {
            return errcode;
        }
        else
            return 0;
    }

    bool Fs::fileExists(const string& path)
    {
        return _access(path.c_str(), 04) == 0;
    }
}
