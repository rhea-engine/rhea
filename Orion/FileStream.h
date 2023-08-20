#ifndef ORION_FILESTREAM_H
#define ORION_FILESTREAM_H

#include "ResourceStream.h"

#include <string>
#include <memory>
#include <stdio.h>
#include <functional>

namespace Orion
{
	class FileResourceStream : public ResourceStream
	{
	public:
		FileResourceStream(std::string path);
		FileResourceStream(std::string path, bool accessible, size_t length, time_t lastmod);
		~FileResourceStream();

		void SetMode(int mode);
		void Open();
		void Close();

		size_t Write(const void* data, size_t length);
		size_t Read(void* dest, size_t length);
		size_t Read(Psio::IOBuffer* buf);
		void ReadAsync(Psio::IOBuffer* buf);
		std::string getLine();
		bool eof();
		int error();

		size_t getLength() { return m_length; }
		time_t getLastmod() { return m_lastmod; }

		static std::unique_ptr<ResourceStream> getStream(std::string path);

	protected:
		FILE* f;
		int status;
		bool read;
		bool write;
		size_t pos;
		void* buf;

		size_t m_length;
		std::string m_path;
		time_t m_lastmod;
		bool m_accessible;
	};
}

#endif // !ORION_FILESTREAM_H
