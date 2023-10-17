#include "IO.h"
#include <Psio/Log.h>

namespace Psio
{
	IO::~IO()
	{
	}

	void IO::RunLoop()
	{
		// iterate over pipe and chck if they are ready for read or write
		for (auto it = m_pipes.begin(); it != m_pipes.end();)
		{
			// check if pipe is ready for read
			if (it->second.flags & IO::IOFLAG::READ)
			{
#ifdef _WIN32
				LPDWORD b_available = 0;
				if (PeekNamedPipe(it->second.pipe_handle, NULL, 0, NULL, b_available, NULL) == 0) {
					if (b_available > 0) {
						it->second.handler(IO::IOFLAG::READ);
						if (it->second.once) {
							m_pipes.erase(it++);
							continue;
						}
					}
				}
#endif
			}
			// check if pipe is ready for write
			if (it->second.flags & IO::IOFLAG::WRITE)
			{
#ifdef _WIN32
				// we could use waitnamedpipe for this, but propably we don't need it
				// because write is not asynchronous
				it->second.handler(IO::IOFLAG::WRITE);
				if (it->second.once) {
					m_pipes.erase(it++);
					continue;
				}
#endif
			}
			it++;
		}

	}

	void IO::Watch(PIPE_HANDLE pipe_h, int flags, bool once, CompletionHandler handler)
	{
		DLOG(LOG_DEBUG) << "watch pipe " << pipe_h << " with flags " << flags;
		m_pipes[pipe_h] = { pipe_h, flags, handler, once };
	}

	void IO::UnWatch(PIPE_HANDLE pipe_h)
	{
		// check all pipe descs
		auto it = m_pipes.find(pipe_h);
		if (it != m_pipes.end()) {
			DLOG(LOG_DEBUG) << "unwatch pipe " << pipe_h;
			m_pipes.erase(it);
		}
	}
}