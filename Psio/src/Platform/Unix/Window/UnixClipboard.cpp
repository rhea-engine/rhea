#include "UnixClipboard.h"

namespace psio
{
	bool UnixClipboard::setText(std::string_view text)
	{
		// Clipboard functionality not implemented for Unix backend
		return false;
	}

	bool UnixClipboard::getText(std::string& text)
	{
		// Clipboard functionality not implemented for Unix backend
		return false;
	}
}