#ifndef PSIO_INPUTMAP_H
#define PSIO_INPUTMAP_H

#include "InputEvent.h"
#include <string>

namespace Psio
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
