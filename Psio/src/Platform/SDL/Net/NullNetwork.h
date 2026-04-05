#ifndef PSIO_NULL_NETWORK_H
#define PSIO_NULL_NETWORK_H

#include <Psio/Net/Network.h>

namespace psio
{
	class NullNetwork : public Network
	{
	public:
		NullNetwork(Engine* engine) :
			Network(engine)
		{
		}

		void Init() override {}

		int SysErr(int err) override {
			return 0;
		}
	};
}

#endif // !PSIO_NULL_NETWORK_H
