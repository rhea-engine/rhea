/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Fs/Fs.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <climits>
#include <Psio/Log.h>

using std::string;

namespace psio
{
    string Fs::getAppDataDir()
    {
        if (!m_app_dir.empty())
            return m_app_dir;

        const char* xdg_data_home = getenv("XDG_DATA_HOME");
        string res;

        if (xdg_data_home && xdg_data_home[0] != '\0') {
            res = xdg_data_home;
        } else {
            const char* home = getenv("HOME");
            if (!home) {
                struct passwd* pw = getpwuid(getuid());
                if (pw)
                    home = pw->pw_dir;
            }
            
            if (home) {
                res = string(home) + "/.local/share";
            } else {
                res = ".";
            }
        }
        
        res += string("/") + m_appname;
        m_app_dir = res;

        if (!fileExists(m_app_dir)) {
            if (mkdir(m_app_dir.c_str(), 0755) != 0) {
                LOG(LOG_ERROR) << "could not create app data folder '" << m_app_dir << "': " << strerror(errno);
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

        // if already found return it
        if (res.length())
            return res;

        // try local
        res = string("Data");
        if (Fs::fileExists(res))
            return res;

        // try relative to executable
        char buf[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
        if (len != -1) {
            buf[len] = '\0';
            res = buf;
            size_t pos = res.find_last_of("/");
            if (pos != string::npos) {
                res = res.substr(0, pos);
                res += string("/Data");
                if (Fs::fileExists(res))
                    return res;
            }
        }

        // try /usr/share
        res = string("/usr/share/") + m_appname + "/Data";
        if (Fs::fileExists(res))
            return res;

        // try /usr/local/share
        res = string("/usr/local/share/") + m_appname + "/Data";
        if (Fs::fileExists(res))
            return res;

        return "";
    }

    std::string Fs::getHomeDir()
    {
        if (m_home.length())
            return m_home;

        const char* home_env = getenv("HOME");
        if (!home_env) {
            struct passwd* pw = getpwuid(getuid());
            if (pw)
                home_env = pw->pw_dir;
        }

        if (home_env)
            m_home = std::string(home_env);
        else
            m_home = ".";
       
        if (!fileExists(m_home)) {
            LOG(LOG_ERROR) << "could not get users home";
            return "";
        }

        return m_home;
    }

    int Fs::fileOpen(FILE** f, string filename, int mode)
    {
        const char* accessMode = "";

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
            fprintf(stderr, "Fs::fileOpen: Unknown mode\n");
        }

        *f = fopen(filename.c_str(), accessMode);
        if (*f == nullptr) {
            return errno;
        }
        return 0;
    }

    bool Fs::fileExists(const string& path)
    {
        return access(path.c_str(), R_OK) == 0;
    }

    bool Fs::createDirectory(const std::string& path)
    {
        if (mkdir(path.c_str(), 0755) != 0) {
            LOG(LOG_ERROR) << "could not create folder: " << strerror(errno);
            return false;
        }
        return true;
    }

    class UnixDirectoryHandle : public Fs::DirectoryHandle
    {
    public:
        ~UnixDirectoryHandle() {
            if (dir)
                closedir(dir);
        }

		bool hasEntries() const override {
			return entry != nullptr;
		}
        
        DIR* dir = nullptr;
        struct dirent* entry = nullptr;
        std::string base_path;
    };

    Fs::DirectoryHandle* Fs::openDirectory(std::string path)
    {        
        UnixDirectoryHandle* fh = new UnixDirectoryHandle();
        fh->base_path = path;
        fh->dir = opendir(path.c_str());
        
        if (fh->dir == nullptr) {
            delete fh;
            return nullptr;
        }
        
        // Read first entry
        fh->entry = readdir(fh->dir);        
        
        return fh;
    }

    int Fs::readEntry(DirectoryHandle* handle, FileAttributes* attributes)
    {
        UnixDirectoryHandle* fh = static_cast<UnixDirectoryHandle*>(handle);
        
        if (fh->entry == nullptr)
            return -1;
        
        attributes->name = fh->entry->d_name;
        
        // Get file stats
        std::string full_path = fh->base_path + "/" + attributes->name;
        struct stat st;
        if (stat(full_path.c_str(), &st) == 0) {
            attributes->is_dir = S_ISDIR(st.st_mode);
            attributes->size = st.st_size;
        } else {
            // Fallback to d_type if stat fails
            attributes->is_dir = (fh->entry->d_type == DT_DIR);
            attributes->size = 0;
        }
        
        return 0;
    }

    bool Fs::nextEntry(DirectoryHandle* handle) 
    {
        UnixDirectoryHandle* fh = static_cast<UnixDirectoryHandle*>(handle);
        fh->entry = readdir(fh->dir);
        return fh->entry != nullptr;
    }

	bool Fs::rename(const std::string& old_path, const std::string& new_path)
	{
		if (::rename(old_path.c_str(), new_path.c_str()) != 0) {
			LOG(LOG_ERROR) << "could not rename file: " << strerror(errno);
			return false;
		}
		return true;
	}

	bool Fs::removeFile(const std::string& path)
	{
		if (unlink(path.c_str()) != 0) {
			LOG(LOG_ERROR) << "could not remove file: " << strerror(errno);
			return false;
		}
		return true;
	}

	bool Fs::removeDirectory(const std::string& path)
	{
		if (rmdir(path.c_str()) != 0) {
			LOG(LOG_ERROR) << "could not remove directory: " << strerror(errno);
			return false;
		}
		return true;
	}

	std::string Fs::getExecutableDir()
	{
#if defined(__APPLE__)
        uint32_t size = 0;
        _NSGetExecutablePath(nullptr, &size);

        if (size == 0)
            return {};

        std::vector<char> buffer(size);
        if (_NSGetExecutablePath(buffer.data(), &size) != 0)
            return {};

        std::error_code error;
        auto executable_path = std::filesystem::canonical(
            std::filesystem::path(buffer.data()), error);

        if (error)
            return {};

        return executable_path.parent_path();

#elif defined(__linux__)
		char buf[PATH_MAX];
		ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
		if (len != -1) {
			buf[len] = '\0';
			std::string res = buf;
			size_t pos = res.find_last_of("/");
			if (pos != std::string::npos) {
				res = res.substr(0, pos);
				return res;
			}
		}
#else
        return {};
#endif
    }
}