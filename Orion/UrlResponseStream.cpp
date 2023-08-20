#include "UrlResponseStream.h"

namespace Orion
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