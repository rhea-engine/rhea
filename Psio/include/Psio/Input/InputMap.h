/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_INPUTMAP_H
#define PSIO_INPUTMAP_H

#include "InputEvent.h"
#include <string>

namespace psio
{
	

	// maps an input event to an action
	class InputMap
	{
	public:
		void map(InputEvent* evt);
	protected:

	};
}

#endif // !PSIO_INPUTMAP_H
