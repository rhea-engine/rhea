#include "os/Fs.h"
#include "FileStream.h"

namespace Orion
{
	FileResourceStream::FileResourceStream(string path, int mode)
	{
		if (mode | Fs::READ)
			read = true;
		else
			read = false;
		if (mode | Fs::WRITE)
			write = true;
		else
			write = false;

		status = Fs::fileOpen(&f, path, mode);
	}

	FileResourceStream::~FileResourceStream()
	{
	}

	void FileResourceStream::Close()
	{
		fclose(f);
	}

	size_t FileResourceStream::Write(void* data, size_t length)
	{
		if (status || !write)
			return -1;

		int written = fwrite(data, 1, length, f);

		return written;
	}

	size_t FileResourceStream::Read(void* dest, size_t length)
	{
		if (status || !read)
			return -1;

		int read = fread_s(dest, length, 1, length, f);
		if (read < length) {
			if (feof) {
				length = read;
			}
			else
				return ferror(f);
		}
		return length;
	}

	string FileResourceStream::getLine()
	{		
		char *r, buf[4096];

		r = fgets(buf, 4096, f);
		if (!r) {
			return "";
		}
		
		return string(buf);
	}

	bool FileResourceStream::eof()
	{
		return feof(f) != 0;
	}
}