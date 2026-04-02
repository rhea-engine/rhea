#ifndef PSIO_WIN32_NETWORK_H
#define PSIO_WIN32_NETWORK_H

#include <Psio/Net/Network.h>

namespace Psio
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
