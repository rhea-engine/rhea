#pragma once

#include <Psio/Window/Clipboard.h>
#include <string>
#include <string_view>

namespace psio
{
	class UnixClipboard : public Clipboard
	{
	public:
		using Clipboard::Clipboard;
		~UnixClipboard() override = default;

		// set the clipboard text, returns false if the operation failed
		bool setText(std::string_view text) override;
		// get the clipboard text, returns false if no text is available or the operation failed
		bool getText(std::string& text) override;
	};
}