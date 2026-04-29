/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/IPC/NamedPipe.h>

#include <iostream>

#ifdef __linux__
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#endif // __linux__


namespace psio
{
	bool NamedPipe::StartServer()
	{
#ifdef _WIN32		
		m_pipe_handle = CreateNamedPipe((LPSTR)m_pipe_name.c_str(), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE, 1, 0, 0, 0, NULL);
		if (m_pipe_handle == INVALID_HANDLE_VALUE) {
			std::cerr << "Error creating Named Pipe" << std::endl;
				return false;
		}
		
		if (!ConnectNamedPipe(m_pipe_handle, NULL)) {
			CloseHandle(m_pipe_handle);
				std::cerr << "Error connecting Named Pipe" << std::endl;
				return false;
		}
		return true;
#elif defined(__linux__)
		int m_server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
		if (m_server_socket == -1) {
			perror("socket");
			return false;
		}
		struct sockaddr_un server_address;
		memset(&server_address, 0, sizeof(struct sockaddr_un));
		server_address.sun_family = AF_UNIX;		
		strcpy(server_address.sun_path, m_pipe_name.c_str()); // Set a unique path

		if (bind(m_server_socket, (struct sockaddr*)&server_address, sizeof(struct sockaddr_un)) == -1) {
			perror("bind");
			return false;
		}

		if (listen(m_server_socket, SOMAXCONN) == -1) {
			perror("listen");
			return false;
		}		
		return true;
#else
		return false;
#endif
	}

	bool NamedPipe::AcceptClient()
	{
#ifdef _WIN32
		if (ConnectNamedPipe(m_pipe_handle, NULL)) {
			return true;
		}
		else {
			std::cerr << "Failed to accept client connection." << std::endl;
			return false;
		}
#else		 
		if (m_remote_socket != -1) {
			perror("already connected");
			return false;
		}

		struct sockaddr_un clientAddress;
		socklen_t clientAddressLength = sizeof(struct sockaddr_un);
		m_remote_socket = accept(m_server_socket, (struct sockaddr*)&clientAddress, &clientAddressLength);
		if (m_remote_socket == -1) {
			perror("accept");
			return false;
		}
		return true;
#endif
	}

	bool NamedPipe::ConnectToServer()
	{
#if defined(_WIN32)
		m_pipe_handle = CreateFile(
			m_pipe_name.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
		);

		if (m_pipe_handle != INVALID_HANDLE_VALUE) {
			return true;
		}
		else {
			std::cerr << "Failed to connect to the server." << std::endl;
			return false;
		}
#else
		if (m_remote_socket != -1) {
			perror("already connected");
			return false;
		}

		m_remote_socket = socket(AF_UNIX, SOCK_STREAM, 0);
		if (m_remote_socket == -1) {
			perror("socket");
			return false;
		}

		struct sockaddr_un serverAddress;
		serverAddress.sun_family = AF_UNIX;
		strncpy(serverAddress.sun_path, m_pipe_name.c_str(), sizeof(serverAddress.sun_path) - 1);

		if (connect(m_remote_socket, (struct sockaddr*)&serverAddress, sizeof(struct sockaddr_un)) == -1) {
			perror("connect");
			return false;
		}

		return true;
#endif
	}

	void NamedPipe::Close()
	{
		if (m_is_open) {
#ifdef _WIN32
			CloseHandle(m_pipe_handle);
#elif defined(__linux__)
			if (m_server_socket != -1)
				close(m_server_socket);
			if (m_remote_socket != -1)
				close(m_remote_socket);
#endif // __linux__
			m_is_open = false;
		}
	}

	NamedPipe::~NamedPipe()
	{
		Close();
	}
}