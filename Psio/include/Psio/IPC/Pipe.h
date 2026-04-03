#ifndef PSIO_IO_PIPE_H
#define PSIO_IO_PIPE_H
#include <functional>
#include "../Net/Platform.h"
#include "IO.h"

namespace Psio
{
	class IOBuffer;

	class Pipe
	{
	public:
		Pipe(IO* io) : m_io(io), m_pipe_handle(HANDLE_PIPE_INVALID), m_is_open(false) {}
		virtual ~Pipe();

		typedef std::function<void(int)> CompletionHandler;

		void SetPipe(PIPE_HANDLE pipe_handle);

		// close the pipe
		virtual void Close();

		// write to pipe
		virtual int Write(IOBuffer* buf, Pipe::CompletionHandler handler);

		// read from pipe
		virtual int Read(IOBuffer* buf, Pipe::CompletionHandler handler);

		// Connection Info	
		
	protected:
		IO* m_io;
		PIPE_HANDLE m_pipe_handle;
		bool m_is_open;
	};
}

#endif // !PSIO_IO_PIPE_H