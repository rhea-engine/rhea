/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Net/Socket/Socket.h>

namespace psio
{
	
	
	int Socket::GetLastError()
	{
		return WSAGetLastError();
	}


}