/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Psio/Fs/Fs.h>
#include <Orion/FileStream.h>
#include <Orion/FileCollection.h>

#ifdef _WIN32
#include <sys/stat.h>
#define STAT _stat
#define FSTAT_STRUCT struct _stat
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
#endif
#else
#include <sys/stat.h>
#define STAT stat
#define FSTAT_STRUCT struct stat
#endif
#include <Psio/Log.h>

namespace orion
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
		FSTAT_STRUCT buf;

		int result = STAT(path.c_str(), &buf);
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
		FSTAT_STRUCT buf;

		int result = STAT(path.c_str(), &buf);
		if (result != 0)
			return nullptr;

		// directory?
		if (S_ISDIR(buf.st_mode)) {
			return std::make_unique<FileCollection>(path);
		}
		else {
			return std::make_unique<FileResourceStream>(path, true, buf.st_size, buf.st_mtime);
		}
	}

	void FileResourceStream::SetMode(int mode)
	{
		if (mode & psio::Fs::READ)
			read = true;
		else
			read = false;
		if (mode & psio::Fs::WRITE)
			write = true;
		else
			write = false;
		m_mode = mode;
	}

	bool FileResourceStream::Open()
	{
		if (m_state == psio::Stream::StreamState::STREAM_OPEN)
			return true;
		status = psio::Fs::fileOpen(&f, m_path, m_mode);
		if (!status) {
			m_state = psio::Stream::StreamState::STREAM_OPEN;
			return true;
		}
		return false;
	}

	FileResourceStream::~FileResourceStream()
	{
		Close();
	}

	void FileResourceStream::Close()
	{
		if (m_state == psio::Stream::StreamState::STREAM_OPEN) {
			fclose(f);			
			m_state = psio::Stream::StreamState::STREAM_CLOSED;
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

		size_t bytesRead = fread(dest, 1, length, f);

		if (bytesRead < length && ferror(f))
			return 0;

		return bytesRead;
	}

	size_t FileResourceStream::Read(psio::IOBuffer* buf)
	{
		size_t size = 0;

		if (m_state != psio::Stream::StreamState::STREAM_OPEN) {
			// open for reading
			m_mode |= psio::Fs::READ;
			read = true;
			if (!Open()) {
				doError(-1);
				return 0;
			}
		}

		if (status || !read || !f)
			return 0;

		size_t buflen = buf->FreeBufSize();
		size_t bytes_read = fread(buf->writer(), 1, buflen, f);
		do {
			buf->Produce(bytes_read);
			size += bytes_read;

			if (bytes_read < buflen) {
				if (ferror(f))
					return 0;
				break;
			}

			if (buf->FreeBufSize() < 8 * 1024) {
				buf->Grow(64 * 1024);
			}

			buflen = buf->FreeBufSize();
			bytes_read = fread(buf->writer(), 1, buflen, f);
		} while (true);
		return size;
	}

#define CHUNK_LEN 64*1024
	// was 8192

	void FileResourceStream::ReadAsync(psio::IOBuffer* buf)
	{		
		size_t bytes_read;
		size_t size = 0;

		// check if file is open for reading
		if (m_state != psio::Stream::StreamState::STREAM_OPEN) {
			// open for reading
			m_mode |= psio::Fs::READ;
			read = true;
			if (!Open()) {
				doError(-1);
				return;
			}
		}

		if (status < 0)
			return;

		// read in the iobuffer
		// start with the remaining free buffer
		// grow the buffer by CHUNK_LEN bytes for further reads
		// repeat until we reach eof file or an error occurs
		// this is synchronous!
		// TODO add asynchronmous file access

		do {
			size_t buflen = buf->FreeBufSize();

			// Ensure we have enough buffer space
			if (buflen < CHUNK_LEN) {
				buf->Grow(CHUNK_LEN);
				buflen = buf->FreeBufSize();
			}

			bytes_read = Read(buf->writer(), buflen);

			if (bytes_read > 0) {
				if (m_data_handler)
					m_data_handler(bytes_read);
				buf->Produce(bytes_read);
				size += bytes_read;
			}

			// Check for errors
			if (bytes_read == 0) {
				if (ferror(f)) {
					LOG(LOG_ERROR) << "Error reading stream\n";
					doError(-1);
					return;
				}
				// EOF reached
				break;
			}
		} while (!feof(f));

		doEnd(size);		
	}

	void FileResourceStream::doEnd(size_t size)
	{
		m_state = psio::Stream::StreamState::STREAM_CLOSED;
		if (m_end_handler)
			m_end_handler(size);
		Close();
	}

	void FileResourceStream::doError(int status)
	{
		if (status < 0) {
			m_state = psio::Stream::StreamState::STREAM_ERROR;
			if (m_error_handler)
				m_error_handler(status);
		}
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