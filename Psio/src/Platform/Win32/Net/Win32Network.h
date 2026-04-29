/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_WIN32_NETWORK_H
#define PSIO_WIN32_NETWORK_H

#include <Psio/Net/Network.h>

namespace psio
{
	class NetworkWin32 : public Network
	{
	public:
		NetworkWin32(Engine* engine) :
			Network(engine),
			m_initialized(false)
		{
		}
		void Init() override;
		int SysErr(int err) override;

	private:
		// Windows Sockets API initialization and cleanup
		bool m_initialized;
	};
}

#endif // !PSIO_WIN32_NETWORK_H
