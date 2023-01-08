#include "Socket.h"

namespace Psio
{
	
	
	int Socket::GetLastError()
	{
		return WSAGetLastError();
	}


}