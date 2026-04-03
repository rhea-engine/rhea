#include <Psio/Net/Network.h>
#include <Psio/Net/Socket/TCPSocket.h>
#include <Psio/Log.h>

#include <cstdio>

#define DEFAULT_WAIT    30000

#ifdef WIN32

#endif

namespace Psio
{
	void Network::RunLoop()
	{
#ifdef HAVE_POLL // WSAPOLL is broken - we must use select

		WSAPOLLFD fdarray = { 0 };

		std::unordered_map<SOCKET, sockitem> pending_sockets = m_sockets;

		// check all file descs
		auto it = pending_sockets.begin();
		while (it != pending_sockets.end()) {
			
			sockitem item = it->second;

			//Call WSAPoll for read- or writeability on connecting socket depending on flag
			fdarray.fd = item.sock;
			if (item.flags == IOFLAG::WRITE)
				fdarray.events = POLLWRNORM;
			else
				fdarray.events = POLLRDNORM;

			int ret = WSAPoll(&fdarray,
				1,
				0
			);
			if (ret == SOCKET_ERROR) {
				return;
			}
			if (ret) {
				// remove watcher
				if (item.once)
					UnWatch(item.sock);
				else
					++it;
				// data ready
				item.handler(ret);
			}
			else
				++it;
		}
#endif

		fd_set fd_read;
		fd_set fd_write;
		fd_set fd_except;
		FD_ZERO(&fd_read);
		FD_ZERO(&fd_write);
		FD_ZERO(&fd_except);

		std::unordered_map<SOCKET, sockitem> pending_sockets = m_sockets;
		int nfds = 0;

		// check all file descs
		auto it = pending_sockets.begin();
		while (it != pending_sockets.end()) {

			sockitem item = it->second;
			SOCKET sock_fd = item.sock;

			if (item.flags)
				if (int(sock_fd) > nfds) nfds = int(sock_fd);

			if (item.flags & IOFLAG::WRITE)
				FD_SET(sock_fd, &fd_write);
			if (item.flags & IOFLAG::READ)
				FD_SET(sock_fd, &fd_read);
			if (item.flags & IOFLAG::EXCEPT)
				FD_SET(sock_fd, &fd_except);

			++it;
		}

		// no open fds?
		if (!nfds) return;

		struct timeval tv;
		tv.tv_sec = (long)0;
		tv.tv_usec = (long)0;
		int ret = ::select(nfds + 1, &fd_read, &fd_write, &fd_except, &tv);
		if (ret > 0) {

			auto it = pending_sockets.begin();
			while (it != pending_sockets.end()) {

				sockitem item = it->second;
				SOCKET sock_fd = item.sock;

				// check if socket is ready and if yes call watcher

				// CHECK: Do we need different watchers?
				int res = 0;
				if (FD_ISSET(sock_fd, &fd_read))
					res = Network::IOFLAG::READ;
				if (FD_ISSET(sock_fd, &fd_write))
					res |= Network::IOFLAG::WRITE;
				if (FD_ISSET(sock_fd, &fd_except))
					res |= Network::IOFLAG::EXCEPT;
				if (res)
				{
					// remove watcher
					if (item.once)
						UnWatch(item.sock);

						++it;
					// data ready
					item.handler(res);
				}
				else
					++it;
			}
		}

	}

	void Network::SetSetting(std::string &setting, std::string &value)
	{
		m_settings[setting] = value;
	}

	std::string Network::GetSetting(std::string setting)
	{
		return m_settings[setting];
	}

	TCPSocket* Network::CreateTCPSocket()
	{
		TCPSocket* sock = new TCPSocket(this);
		return sock;
	}

	void Network::Watch(SOCKET sock, int flags, bool once, CompletionHandler handler)
	{
		DLOG(LOG_DEBUG) << "watch socket " << sock << " with flags " << flags;
		m_sockets[sock]={sock, flags, handler, once};
	}

	void Network::UnWatch(SOCKET sock)
	{
		// check all file descs
		auto it = m_sockets.find(sock);
		if (it != m_sockets.end()) {
			DLOG(LOG_DEBUG) << "unwatch socket " << sock;
			m_sockets.erase(it);
		}
	}
}