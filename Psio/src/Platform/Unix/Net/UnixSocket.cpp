#include <errno.h>

#include <Psio/Net/Socket/Socket.h>

namespace Psio
{
	int Socket::GetLastError()
	{
		return errno;
	}


}