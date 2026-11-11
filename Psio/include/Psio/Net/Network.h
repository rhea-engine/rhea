/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_NET_NETWORK_H
#define PSIO_NET_NETWORK_H

#include "../Psio.h"
#include "Platform.h"

#include <functional>
#include <vector>
#include <unordered_map>
#include <string>
#include <chrono>
#include <cstdint>
#include <cstddef>
#include <cinttypes>

namespace psio
{
	class TCPSocket;

	class Network
	{
	public:
		Network(Engine* engine) :
			m_engine(engine)
		{}
		Network() :
			m_engine(nullptr)
		{}
		virtual ~Network() = default;

		typedef std::function<void(int)> CompletionHandler;

		// startup the network system
		virtual void Init() = 0;

		// run the net main loop once
		void RunLoop();

		// set setting
		void SetSetting(const std::string &setting, const std::string &value);
		// get setting
		std::string GetSetting(std::string setting);

		// helper
		virtual TCPSocket* CreateTCPSocket();

		// register socket to watch
		void Watch(SOCKET sock, int flags, bool once, CompletionHandler handler);
		// unregister socket from watching
		void UnWatch(SOCKET sock);

		enum Err {
			OK = 0,

			IO_PENDING = -1,
			FAILED = -2,
			ABORTED = -3,
			NO_SOCKET = -4,
			NOT_CONNECTED = -5,
			FAULT = -6,
			CONN_RESET = -7,
			TIMEOUT = -8,
			CONN_REFUSED = -9,

			// 3xx HTTP errors

			// stream related
			INVALID_HTTP_RESPONSE = -300,
			TOO_MANY_REDIRECTS = -301,

			// request headers
			REQUEST_URL_INVALID = -310,

			// response header parsing
			RESPONSE_HEADERS_TRUNCATED = -320,
			RESPONSE_HEADERS_TOO_BIG = -321,


		};
		
		/* Normalize system errors 
		*/
		virtual int SysErr(int err) = 0;

		enum IOFLAG {
			READ = 1,
			WRITE = 2,
			EXCEPT = 4
		};

		using Clock = std::chrono::steady_clock;
		using TimerId = std::uint64_t;

		// timer handling

		TimerId scheduleAt(Clock::time_point deadline, std::function<void()> handler);
		void updateTimer(TimerId id, Clock::time_point deadline);
		void dispatchExpiredTimers();
		void cancelTimer(TimerId timer_id);

	protected:

		struct sockitem {
			SOCKET sock = 0;
			int flags = 0;
			CompletionHandler handler;
			bool once = false;
			std::uint64_t watch_id = 0;
		};

		Engine* m_engine;
		std::unordered_map<SOCKET, sockitem> m_sockets;
		std::unordered_map<std::string, std::string> m_settings;

		struct TimerEntry {
			Clock::time_point deadline;
			std::function<void()> handler;
		};
		std::unordered_map<TimerId, TimerEntry> m_timers;
		TimerId m_next_timer_id = 1;

		std::uint64_t m_next_watch_id = 1;
	};
}
#endif // !PSIO_NET_NETWORK_H
