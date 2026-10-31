#pragma once

#include <string>
#include <string_view>
#include <Psio/Psio.h>

namespace psio
{
	class Clipboard
	{
	public:
		Clipboard(Engine* engine) : m_engine(engine) {}
		virtual ~Clipboard() = default;

		// set the clipboard text, returns false if the operation failed
		virtual bool setText(std::string_view text) = 0;

		// get the clipboard text, returns false if no text is available or the operation failed
		virtual bool getText(std::string& text) = 0;
	protected:
		Engine* m_engine = nullptr;
	};
}