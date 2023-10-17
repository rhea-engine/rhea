#ifndef PSIO_IPC_IO_H
#define PSIO_IPC_IO_H

#include <functional>
#include <unordered_map>

#include "../Psio.h"
#include "../Net/Platform.h"

namespace Psio
{
	class IO
	{
	public:
		IO(Engine* engine) : 
			m_engine(engine) 
		{}
		~IO();

		typedef std::function<void(int)> CompletionHandler;

		// run the net main loop once
		void RunLoop();

		// register pipe to watch
		void Watch(PIPE_HANDLE pipe_h, int flags, bool once, CompletionHandler handler);
		
		// unregister pipe from watching
		void UnWatch(PIPE_HANDLE pipe_h);

		enum IOFLAG {
			READ = 1,
			WRITE = 2,
			EXCEPT = 4
		};

	protected:
		struct pipeitem {
			PIPE_HANDLE pipe_handle;
			int flags;
			CompletionHandler handler;
			bool once;
		};

		Engine* m_engine;
		std::unordered_map<PIPE_HANDLE, pipeitem> m_pipes;
	};
}

#endif // !PSIO_IPC_IO_H
