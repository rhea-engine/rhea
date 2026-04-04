#include "UnixNetwork.h"

#include <cstdio>
#include <csignal>

void psio::NetworkUnix::Init()
{
	struct sigaction sa{};
	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;
	if (sigemptyset(&sa.sa_mask) == -1 || sigaction(SIGPIPE, &sa, 0) == -1) {
		perror("failed to ignore SIGPIPE; sigaction");
		exit(EXIT_FAILURE);

	}
}

int psio::NetworkUnix::SysErr(int err)
{
	if (err <= 0)
		return err;
	switch (err) {
	case EWOULDBLOCK:
		return Err::IO_PENDING;
	case EFAULT:
		return Err::FAULT;
	case ENOTSOCK:
		return Err::NO_SOCKET;
	case ENOTCONN:
		return Err::NOT_CONNECTED;
	case ECONNABORTED:
		return Err::ABORTED;
	case ECONNRESET:
		return Err::CONN_RESET;
	default:
		return Err::FAILED;
	}
}
