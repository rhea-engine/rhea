#ifndef ORION_FILESTREAM_H
#define ORION_FILESTREAM_H

#include "ResourceStream.h"

#include <string>
#include <stdio.h>

namespace Orion
{
	class FileResourceStream : public ResourceStream
	{
	protected:
		FILE* f;
		int status;
		bool read;
		bool write;
		size_t pos = 0;
		void* buf = NULL;
	public:
		FileResourceStream(string path, int mode);
		~FileResourceStream();

		void Close();

		size_t Write(void* data, size_t length);
		size_t Read(void* dest, size_t length);
		string getLine();
		bool eof();
	};
}

#endif // !ORION_FILESTREAM_H
