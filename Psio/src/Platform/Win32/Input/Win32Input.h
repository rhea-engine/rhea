#ifndef PSIO_INPUT_WIN32_H
#define PSIO_INPUT_WIN32_H

#include <Psio/Input/Input.h>

namespace Psio
{
	class InputWin32 : public Input
	{
	public:
		InputWin32() = default;
		~InputWin32() override = default;
		void update() override;
	};
		
}
#endif // !PSIO_INPUT_WIN32_H
