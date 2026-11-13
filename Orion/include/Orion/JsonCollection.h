#ifndef ORION_JSONCOLLECTION_H
#define ORION_JSONCOLLECTION_H

#include <Orion/Collection.h>
#include <Orion/RequestError.h>
#include <Orion/ResourceAddress.h>
#include <memory>
#include <vector>

namespace orion
{
	class JsonCollection : public Collection
	{
    public:     
        Entry* nextEntry() override; 

        static orion::Error parse(char* buf, size_t buflen, 
			const orion::ResourceAddress& effective_address,
            std::unique_ptr<JsonCollection>& collection);

		// resource stream no-ops
		size_t getLength() { return 0; }

		// open for reading
		// used internally for async functions or must be called before using sync functions
		bool Open() { return true; };

		// write length bytes to resource
		// returns actual bytes written
		size_t Write(const void* data, size_t length) { return 0; }

		// read from resource up to length bytes

		// returns actual bytes read
		size_t Read(void* dest, size_t length) { return 0; }

		// get the next line as string
		std::string getLine() { return std::string(""); }

		// return eof condition
		bool eof() { return m_state == StreamState::STREAM_CLOSED; }

		// Close the stream
		void Close() { m_state = StreamState::STREAM_CLOSED; }

		// psio::Stream functions
		void ReadAsync(psio::IOBuffer* buf) {};

		// set internal mode
		void SetMode(int mode) { m_mode = mode; }

    private:
        JsonCollection(char* buf, size_t buflen, const orion::ResourceAddress& effective_address);

        std::vector<Entry> m_entries;
        size_t m_next_entry = 0;

		Error m_parse_error = Error::OK;
	};
}

#endif // ORION_JSONCOLLECTION_H