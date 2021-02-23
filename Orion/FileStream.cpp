#include "os/Fs.h"
#include "FileStream.h"

namespace Orion
{
	FileResourceStream::FileResourceStream(string path, int mode) :
		f(nullptr),		
		pos(0),
		buf(nullptr),
		ResourceStream()
	{
		if (mode & Fs::READ)
			read = true;
		else
			read = false;
		if (mode & Fs::WRITE)
			write = true;
		else
			write = false;

		status = Fs::fileOpen(&f, path, mode);
		if (!status)
			m_isOpen = true;
	}

	FileResourceStream::~FileResourceStream()
	{
		Close();
	}

	void FileResourceStream::Close()
	{
		if (m_isOpen) {
			fclose(f);
			m_isOpen = false;
		}
	}

	size_t FileResourceStream::Write(void* data, size_t length)
	{
		if (status || !write || !f)
			return 0;

		size_t written = fwrite(data, 1, length, f);

		return written;
	}

	size_t FileResourceStream::Read(void* dest, size_t length)
	{
		if (status || !read || !f)
			return 0;

		size_t read = fread_s(dest, length, 1, length, f);
		if (read < length) {
			if (feof(f)) {
				length = read;
			}
			else
				return 0;
		}
		return length;
	}

#define CHUNK_LEN 8192

	size_t FileResourceStream::ReadBuffer(char** dest)
	{
		char* buf = (char*)malloc(CHUNK_LEN);
		char* temp;
		size_t buflen = CHUNK_LEN;
		size_t tmplen;
		size_t read;
		size_t size = 0;

		if (status || !m_isOpen || !f) {
			free(buf);
			return 0;
		}

		read = Read(buf, CHUNK_LEN);
		size += read;
		while (read == CHUNK_LEN) {
			tmplen = buflen;
			buflen += CHUNK_LEN;
			temp = (char*)realloc(buf, buflen);
			if (temp != NULL)
				buf = temp;
			else {
				free(buf);
				return -1;
			}
			read = Read(buf + tmplen, CHUNK_LEN);
			size += read;
		}
		if (f && read == 0) {
			if (!feof(f))
				printf("Error reading stream");
		}
		*dest = buf;
		return size;
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
	
	int FileResourceStream::error()
	{
		if (!f)
			return -1;
		if (feof(f))
			return 0;
		return std::ferror(f);
	}
}