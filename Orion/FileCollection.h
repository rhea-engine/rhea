#ifndef ORION_FILECOLLECTION_H
#define ORION_FILECOLLECTION_H

#include <memory>

#include "os/Fs.h"
#include "Collection.h"

namespace Orion
{
	class FileCollection : public Collection
	{
	public:
		FileCollection(std::string path);
		~FileCollection();

		Entry* nextEntry();

		// resource stream no-ops
		size_t getLength() { return 0; }

		// open for reading
		// used internally for async functions or must be called before using sync functions
		void Open() {};

		// write length bytes to resource
		// returns actual bytes written
		size_t Write(const void* data, size_t length) { return 0; }

		// read from resource up to length bytes

		// returns actual bytes read
		size_t Read(void* dest, size_t length) { return 0; }

		// get the next line as string
		std::string getLine() { return std::string(""); }

		// return eof condition
		bool eof() { return m_end; }

		// Close the stream
		void Close();

		// Psio::Stream functions
		void ReadAsync(Psio::IOBuffer* buf) {};

		// set internal mode
		void SetMode(int mode) { m_mode = mode; }

	protected:
		std::unique_ptr<Fs::DirectoryHandle> m_handle;
		bool m_end;
	};
}

#endif // !ORION_FILECOLLECTION_H
