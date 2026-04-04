#ifndef ORION_URLRESPONSESTREAM_H
#define ORION_URLRESPONSESTREAM_H

#include <Orion/ResourceStream.h>
#include <Psio/Net/UrlRequest/UrlResponse.h>
#include <memory>

namespace orion
{
	class UrlResponseStream : public ResourceStream
	{
	public:
		UrlResponseStream(std::unique_ptr<psio::UrlResponse> respone) :
			m_stream(std::move(respone))
		{}
		~UrlResponseStream() {}

		// Forward functions
		void onData(DataHandler handler) {
			if (m_stream) m_stream->onData(handler);
		}

		// called when there is no more data
		void onEnd(EventHandler handler) {
			if (m_stream) m_stream->onEnd(handler);
		}

		// called when there is an error - passes error number
		void onError(ErrorHandler handler) {
			if (m_stream) m_stream->onError(handler);
		}

		void ReadAsync(psio::IOBuffer* buf) {
			if (m_stream) m_stream->ReadAsync(buf);
		}

		// set internal mode
		void SetMode(int mode) {
			if (m_stream) m_stream->SetMode(mode);
		}

		int GetResponseCode() {
			if (m_stream) return m_stream->GetResponseCode();
			else return 502; // Bad Gatway
		}

		// response stream functions

		size_t getLength() {
			if (m_stream) return m_stream->getLength();
			else return 0;
		}

		// open for reading
		// used internally for async functions or must be called before using sync functions
		void Open() {} // non-op for UrlResponse

		// write length bytes to resource
		// returns actual bytes written
		size_t Write(const void* data, size_t length) { return 0; } // non-op for UrlResponse

		// read from resource up to length bytes

		// returns actual bytes read
		size_t Read(void* dest, size_t length);

		// get the next line as string
		std::string getLine();

		// return eof condition
		bool eof();

		// Close the stream
		void Close();

	protected:
		std::unique_ptr<psio::UrlResponse> m_stream;
	};
}

#endif // !ORION_URLRESPONSESTREAM_H
