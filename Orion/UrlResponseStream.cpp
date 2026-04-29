/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Orion/UrlResponseStream.h>

namespace orion
{
	size_t UrlResponseStream::Read(void* dest, size_t length)
	{
		return 0;
	}

	std::string UrlResponseStream::getLine()
	{
		return std::string("");
	}

	bool UrlResponseStream::eof()
	{
		return false;
	}

	void UrlResponseStream::Close()
	{
		m_stream.reset();
	}
}