#include "os/Fs.h"
#include "FileStream.h"
#include "FileCollection.h"

namespace Orion
{
	FileResourceStream::FileResourceStream(std::string path) :
		f(nullptr),		
		pos(0),
		buf(nullptr),
		m_path(path),
		write(false),
		read(false),
		status(0),
		ResourceStream()
	{
		struct _stat buf;

		int result = _stat(path.c_str(), &buf);
		if (result != 0) {
			m_accessible = false;
			m_length = 0;
			m_lastmod = 0;
		}
		else {
			m_accessible = true;
			m_length = buf.st_size;
			m_lastmod = buf.st_mtime;
		}	
		m_mode = 0;
	}

	FileResourceStream::FileResourceStream(std::string path, bool accessible, size_t length, time_t lastmod) :
		f(nullptr),
		pos(0),
		buf(nullptr),
		m_path(path),
		write(false),
		read(false),
		status(0),
		m_accessible(accessible),
		m_length(length),
		m_lastmod(lastmod),		
		ResourceStream()
	{}

	// static constructor return filestream or directory
	std::unique_ptr<ResourceStream> FileResourceStream::getStream(std::string path)
	{
		struct _stat buf;

		int result = _stat(path.c_str(), &buf);
		if (result != 0)
			return nullptr;

		// directory?
		if (buf.st_mode & _S_IFDIR) {			
			return std::make_unique<FileCollection>(path);
		}
		else {
			return std::make_unique<FileResourceStream>(path, true, buf.st_size, buf.st_mtime);
		}
	}

	void FileResourceStream::SetMode(int mode)
	{
		if (mode & Fs::READ)
			read = true;
		else
			read = false;
		if (mode & Fs::WRITE)
			write = true;
		else
			write = false;
		m_mode = mode;
	}

	void FileResourceStream::Open()
	{
		status = Fs::fileOpen(&f, m_path, m_mode);
		if (!status)
			m_isOpen = true;
		// TODO: else?
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

	size_t FileResourceStream::Write(const void* data, size_t length)
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

	size_t FileResourceStream::Read(Psio::IOBuffer* buf)
	{
		size_t size = 0;

		if (!m_isOpen) {
			// open for reading
			m_mode |= Orion::Fs::READ;
			read = true;
			Open();
		}

		if (status || !read || !f)
			return 0;

		size_t buflen = buf->FreeBufSize();
		size_t bytes_read = fread_s(buf->writer(), buflen, 1, buflen, f);
		do {
			buf->Produce(bytes_read);
			size += bytes_read;
			if (buf->FreeBufSize() < 8 * 1024) {
				buf->Grow(64 * 1024);
			}
			if (bytes_read < buflen)
				break;
			else
				buflen = buf->FreeBufSize();
			bytes_read = fread_s(buf->writer(), buflen, 1, buflen, f);
		} while (true);
		return size;
	}

#define CHUNK_LEN 64*1024
	// was 8192

	void FileResourceStream::ReadAsync(Psio::IOBuffer* buf)
	{
		char* temp;
		size_t buflen;
		size_t tmplen;
		size_t bytes_read;
		size_t size = 0;

		// check if file is open for reading
		if (!m_isOpen) {
			// open for reading
			m_mode |= Orion::Fs::READ;
			read = true;
			Open();
		}

		if (status < 0)
			return;

		// read in the iobuffer
		// start with the remaining free buffer
		// grow the buffer by CHUNK_LEN bytes for further reads
		// repeat until we read fewer bytes than free buf len which indicates file end
		// this is synchronous!
		// TODO add asynchronmous file access

		buflen = buf->FreeBufSize();
		bytes_read = Read(buf->writer(), buflen);
		if (bytes_read > 0) {
			if (m_data_handler)
				m_data_handler(bytes_read);
			buf->Produce(bytes_read);
			size += bytes_read;
		}
		while (bytes_read == buflen) {

			buf->Grow(CHUNK_LEN);
			buflen = buf->FreeBufSize();

			bytes_read = Read(buf->writer(), buflen);
			if (bytes_read > 0) {
				if (m_data_handler)
					m_data_handler(bytes_read);
				buf->Produce(bytes_read);
				size += bytes_read;
			}
		}
		if (f && bytes_read == 0) {
			if (!feof(f)) {
				printf("Error reading stream\n");
				status = -1;
				if (m_error_handler)
					m_error_handler(status);
				return;
			}
		}
		m_end_handler(size);
		Close();
	}

	std::string FileResourceStream::getLine()
	{		
		char *r, buf[4096];

		r = fgets(buf, 4096, f);
		if (!r) {
			return "";
		}
		
		return std::string(buf);
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