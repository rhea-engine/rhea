#ifndef PSIO_IPC_NAMEDPIPE_H
#define PSIO_IPC_NAMEDPIPE_H

#include "../Net/Platform.h"

#if defined(_WIN32)

#elif defined(__linux__)
#include <unistd.h>
#endif
#include <string>

namespace psio
{
	class NamedPipe
	{
	public:
#ifdef _WIN32
		NamedPipe(std::string prefix) :
			m_is_parent(false),
			m_is_open(false),
			m_pipe_handle(INVALID_HANDLE_VALUE),
			m_pipe_name("\\\\.\\pipe\\" + prefix)
#elif defined(__linux__)
		NamedPipe(std::string prefix) :
			m_is_parent(false), 
			m_is_open(false), 
			m_server_socket(-1), 
			m_remote_socket(-1),
			m_pipe_name("/tmp/" + prefix + "_socket")
#endif
		{}
		~NamedPipe();
		
		bool StartServer();
		bool AcceptClient();
		bool ConnectToServer();
		void Close();

	protected:
		std::string m_pipe_name;
#if defined(_WIN32)
		HANDLE m_pipe_handle;
#elif defined(__linux__)
		int m_server_socket;
		int m_remote_socket;
#endif
		bool m_is_parent;
		bool m_is_open;
	};
}

#endif // !PSIO_IPC_NAMEDPIPE_H
