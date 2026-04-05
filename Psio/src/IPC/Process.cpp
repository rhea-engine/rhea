#include <Psio/IPC/Process.h>

#if defined(_WIN32)
#define NOMINMAX
#include <windows.h> 
#include <tchar.h>
#include <stdio.h> 
#include <strsafe.h>
#elif defined(__linux)
#include <sys/types.h>
#include <unistd.h>
#endif // _WIN32

#include <Psio/IPC/Pipe.h>

psio::Process::~Process()
{
#ifdef _WIN32
	CloseHandle(m_process_handle);
#elif defined(__linux__)

#endif // 

}

bool psio::Process::Fork(std::string exe_name, std::string arg, Pipe* std_in, Pipe* std_out, Pipe* std_err)
{
#ifdef _WIN32	
	std::string cmdline = exe_name + " " + arg;
	PROCESS_INFORMATION proc_info;
	STARTUPINFO start_info;
	BOOL success = FALSE;

    HANDLE child_in_read;
    HANDLE child_in_write;
    HANDLE child_out_read;
    HANDLE child_out_write;
    HANDLE child_err_read;
    HANDLE child_err_write;

	// Set up members of the PROCESS_INFORMATION structure.
    ZeroMemory(&start_info, sizeof(STARTUPINFO));
    start_info.cb = sizeof(STARTUPINFO);
    if (std_in || std_out || std_err) {

        // setup the pipes
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;
 
        if (std_in) {
            // pipe for STDIN
            if (!CreatePipe(&child_in_read, &child_in_write, &saAttr, 0)) {
                printf(TEXT("CreatePipe failed for stdin"));
                return false;
            }
            // Ensure the read handle to the pipe for STDIN is not inherited.
            if (!SetHandleInformation(child_in_write, HANDLE_FLAG_INHERIT, 0)) {
                printf(TEXT("SetHandleInformation failed for stdin"));
                return false;
            }
            start_info.hStdInput = child_in_read;
            std_in->SetPipe(child_in_write);
        }

        // pipe for STDOUT
        if (std_out) {
            if (!CreatePipe(&child_out_read, &child_out_write, &saAttr, 0)) {
                printf(TEXT("CreatePipe failed for stdout"));
                return false;
            }
            // Ensure the read handle to the pipe for STDIN is not inherited.
            if (!SetHandleInformation(child_out_read, HANDLE_FLAG_INHERIT, 0)) {
                printf(TEXT("SetHandleInformation failed for stdout"));
                return false;
            }
            start_info.hStdOutput = child_out_write;
            std_out->SetPipe(child_out_read);
        }

        if (std_err) {
            // pipe for STDERR
            if (!CreatePipe(&child_err_read, &child_err_write, &saAttr, 0)) {
                printf(TEXT("CreatePipe failed for stderr"));
                return false;
            }
            // Ensure the read handle to the pipe for STDIN is not inherited.
            if (!SetHandleInformation(child_err_read, HANDLE_FLAG_INHERIT, 0)) {
                printf(TEXT("SetHandleInformation failed for stderr"));
                return false;
            }
            start_info.hStdError = child_err_write;
            std_err->SetPipe(child_err_read);
        }
        else if (std_out) {
            start_info.hStdError = child_out_write;
        }
    }
    start_info.dwFlags |= STARTF_USESTDHANDLES;

    // Create the child process. 

    success = CreateProcess(
		NULL,          // No module name (use command line)
        (LPSTR)cmdline.c_str(),       // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &start_info,  // STARTUPINFO pointer 
        &proc_info);   // receives PROCESS_INFORMATION 

    // If an error occurs, exit the application. 
    if (!success)
        return false;
    else
    {
        // Close handles to the child process and its primary thread.
        // Some applications might keep these handles to monitor the status
        // of the child process, for example. 

       // CloseHandle(proc_info.hProcess);
        CloseHandle(proc_info.hThread);

        // Close handles to the stdin and stdout pipes no longer needed by the child process.
        // If they are not explicitly closed, there is no way to recognize that the child process has ended.

        if (std_in)
            CloseHandle(child_in_read);
        if (std_out)
            CloseHandle(child_out_write);
        if (std_err)
            CloseHandle(child_err_write);
    
        m_process_handle = proc_info.hProcess;
        return true;
    }
#elif defined(__linux__)
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return false;
    }
    else if (pid == 0) {
        // Child process
  
        execl(exe_name.c_str(), arg.c_str(), (char*)NULL);
        perror("exec");
        exit(1);
    }
    else {
        // Parent process
        m_pid = pid;
		return true;
    }
#endif
}
