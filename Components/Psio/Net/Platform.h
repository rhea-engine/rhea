#ifndef PSIO_NET_PLATFORM_H
#define PSIO_NET_PLATFORM_H

// platform specific defines
// inspired by Panda3d: /panda/src/nativenet/socket_portable.h

#define SA_SIZEOF(addr) (((addr)->sa_family == AF_INET6) ? sizeof(sockaddr_in6) : sizeof(sockaddr_in))

#if defined(CPPPARSER)

typedef unsigned long SOCKET;

#include <sys/socket.h>
#include <netinet/in.h>

#elif defined(_WIN32)

/*
*	WINSOCK
*/

#include <winsock2.h>
#include <ws2tcpip.h>

typedef u_short sa_family_t;

typedef int socklen_t;
const long LOCAL_NONBLOCK = 1;
const long LOCAL_FL_SET = FIONBIO;
const int LOCAL_BLOCKING_ERROR = WSAEWOULDBLOCK;
const int LOCAL_CONNECT_BLOCKING = WSAEWOULDBLOCK;
const int LOCAL_NOTCONNECTED_ERROR = WSAENOTCONN;
const int LOCAL_TIMEOUT_ERROR = WSAETIMEDOUT;
const SOCKET BAD_SOCKET = (SOCKET)-1;

#elif defined(SunOS) || defined(SUNNEW) || defined(IRIX64)

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/filio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int SOCKET;
const SOCKET BAD_SOCKET = 0xffffffff;
#ifndef INADDR_NONE
const INADDR_NONE = -1;
#endif

const long LOCAL_NONBLOCK = 1;
const long LOCAL_FL_SET = FIONBIO;
const int LOCAL_BLOCKING_ERROR = EAGAIN;
const int LOCAL_CONNECT_BLOCKING = EINPROGRESS;

#elif defined(__linux__) || defined(IS_LINUX) || defined(IS_OSX) || defined(IS_FREEBSD)
/*
*	Linux, OSX, FreeBSD
*/
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
// #include <netinetin_systm.h>
#include <netinet/tcp.h>
// #include <netinetip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef int SOCKET;
const SOCKET BAD_SOCKET = -1;

#define  BSDBLOCK


const long LOCAL_NONBLOCK = 1;
// With BSDBLOCK defined, we don't need FIONBIO.  Solaris doesn't provide it.
// const long LOCAL_FL_SET = FIONBIO;
const int LOCAL_BLOCKING_ERROR = EAGAIN;
const int LOCAL_CONNECT_BLOCKING = EINPROGRESS;

#else
No Host Type defined !!
#error  Fatal
#endif


#endif // !PSIO_NET_PLATFORM_H
