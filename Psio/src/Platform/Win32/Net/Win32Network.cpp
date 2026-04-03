#include "Win32Network.h"
#include <Psio/Log.h>

void Psio::NetworkWin32::Init()
{
	WSADATA wsa;
	long rc = WSAStartup(MAKEWORD(2, 0), &wsa);

	if (rc) {
		LOG(LOG_FATAL) << "Network Win32: could not start: " << rc;
	}
}

int Psio::NetworkWin32::SysErr(int err)
{
	if (err <= 0)
		return err;
	switch (err) {
	case WSAEWOULDBLOCK:
		return Err::IO_PENDING;
	case WSAEFAULT:
		return Err::FAULT;
	case WSAENOTSOCK:
		return Err::NO_SOCKET;
	case WSAENOTCONN:
		return Err::NOT_CONNECTED;
	case WSAECONNABORTED:
		return Err::ABORTED;
	case WSAECONNRESET:
		return Err::CONN_RESET;
	default:
		return Err::FAILED;
	}
}
