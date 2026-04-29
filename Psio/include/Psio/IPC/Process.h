/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_IPC_PROCESS_H
#define PSIO_IPC_PROCESS_H
#include <string>
#include "../Net/Platform.h"
#include "Pipe.h"

namespace psio
{
	class Process
	{
	public:
#ifdef _WIN32
		Process() : m_process_handle(NULL) {}
#else
		Process() : m_pid(-1) {}
#endif // _WIN32
		~Process();

		bool Fork(std::string exe_name, std::string arg, Pipe* std_in, Pipe* std_out, Pipe* std_err);
	
	protected:		
#ifdef _WIN32
		HANDLE m_process_handle;
		HANDLE m_child_in_read = NULL;
		HANDLE m_child_in_write = NULL;
		HANDLE m_child_out_read = NULL;
		HANDLE m_child_out_write = NULL;
		HANDLE m_child_err_read = NULL;
		HANDLE m_child_err_write = NULL;		
#else
		pid_t m_pid;
#endif // _WIN32
	};
}

#endif // !PSIO_IPC_PROCESS_H
