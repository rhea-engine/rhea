#include <errno.h>

#include <Psio/Net/Socket/Socket.h>

namespace psio
{
	int Socket::GetLastError()
	{
		return errno;
	}


}