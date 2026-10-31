#pragma once

#include <Psio/Window/Clipboard.h>

namespace psio
{
	class Win32Clipboard : public Clipboard
	{
	public:
		using Clipboard::Clipboard;
		~Win32Clipboard() override = default;

		virtual bool setText(std::string_view text) override;
		virtual bool getText(std::string& text) override;
	};
}