#include <errno.h>

#include "Socket.h"

namespace Psio
{
	int Socket::GetLastError()
	{
		return errno;
	}


}