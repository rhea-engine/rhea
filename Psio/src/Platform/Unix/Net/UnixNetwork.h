/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_UNIX_NETWORK_H
#define PSIO_UNIX_NETWORK_H

#include <Psio/Net/Network.h>

namespace psio
{
	class NetworkUnix : public Network
	{
	public:
		NetworkUnix(Engine* engine) : 
			Network(engine) 
		{}

		// override virtual methods from Network
		void Init() override;
		int SysErr(int err) override;
	};
}
#endif // !PSIO_UNIX_NETWORK_H
