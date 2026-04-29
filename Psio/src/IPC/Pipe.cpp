/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/IPC/Pipe.h>
#include <Psio/Net/IOBuffer.h>

namespace psio
{
	Pipe::~Pipe()
	{
		Close();
	}
		void Pipe::SetPipe(PIPE_HANDLE pipe_handle)
	{
		m_pipe_handle = pipe_handle;
		m_is_open = true;
	}

	void Pipe::Close()
	{
		if (m_is_open) {
#ifdef _WIN32
			CloseHandle(m_pipe_handle);
#else
			close(m_pipe_handle);
#endif
			m_is_open = false;
		}
	}

	int Pipe::Write(IOBuffer* buf, Pipe::CompletionHandler handler)
	{
#ifdef _WIN32
		DWORD dw_written;
		bool success;
		success = WriteFile(m_pipe_handle, buf->reader(), buf->Remaining(), &dw_written, NULL);
		if (success) {
			buf->Consume((int)dw_written);
			handler((int)dw_written);
			return (int)dw_written;
		}
		else {
			int err = GetLastError();
			if (err == ERROR_IO_PENDING) {
				// watch for completion
				m_io->Watch(this, IO::IOFLAG::WRITE, true, [&, handler, buf](int result)->void {
					DWORD dw_written;
					bool success; 
					success = WriteFile(m_pipe_handle, buf->reader(), buf->Remaining(), &dw_written, NULL);
					if (success) {
						buf->Consume((int)dw_written);
						handler((int)dw_written);
					}
					else {
						int err = GetLastError();
						if (err == ERROR_IO_PENDING) {
							// retry
							Write(buf, handler);
						}
						else {
							// TODO get sys error
							// handler(err);
						}
					}
					});
				return -1;
			}
			else {
				// TODO get sys error
				return err;
			}
		}
		
#endif // _WIN32

		return 0;
	}

	int Pipe::Read(IOBuffer* buf, Pipe::CompletionHandler handler)
	{
#ifdef _WIN32
		DWORD dw_read;
		BOOL success = FALSE;
		OVERLAPPED overlapped;
		memset(&overlapped, 0, sizeof(OVERLAPPED));

		success = ReadFile(m_pipe_handle, buf->writer(), buf->FreeBufSize(), &dw_read, &overlapped);
		if (success) {
			buf->Produce((int)dw_read);
			handler((int)dw_read);
			return (int)dw_read;
		}
		else {
			int err = GetLastError();
			if (err == ERROR_IO_PENDING) {
				// watch for completion
				m_io->Watch(this, IO::IOFLAG::READ, true, [&, handler, buf](int result)->void {
					DWORD dw_read;
					bool success;
					OVERLAPPED overlapped;
					memset(&overlapped, 0, sizeof(OVERLAPPED));
					success = ReadFile(m_pipe_handle, buf->writer(), buf->FreeBufSize(), &dw_read, &overlapped);
					if (success) {
						buf->Produce((int)dw_read);
						handler((int)dw_read);
					}
					else {
						int err = GetLastError();
						if (err == ERROR_IO_PENDING) {
							// retry
							Read(buf, handler);
						}
						else {
							// TODO get sys error
							// handler(err);
						}
					}
					});
				return -1;
			}
			else {
				// TODO get sys error
				return err;
			}
		}
#endif // _WIN32
		return 0;
	}
}