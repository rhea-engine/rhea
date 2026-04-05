#ifndef PSIO_STREAM_H
#define PSIO_STREAM_H

#include <functional>
#include <Psio/Net/IOBuffer.h>

namespace psio
{
	class Stream
	{
	public:
		Stream() :
			m_data_handler(nullptr),
			m_end_handler(nullptr),
			m_error_handler(nullptr),
			m_error(0)
		{}
		virtual ~Stream() {}

		// callback types
		typedef std::function<void(size_t)> DataHandler;
		typedef std::function<void(size_t)> EventHandler;
		typedef std::function<void(int)> ErrorHandler;

		// called when data is read - a chunk of the data is passed
		virtual void onData(DataHandler handler) {
			m_data_handler = handler;
		}

		// called when there is no more data
		virtual void onEnd(EventHandler handler) {
			m_end_handler = handler;
		}

		// called when there is an error - passes error number
		virtual void onError(ErrorHandler handler) {
			m_error_handler = handler;
		}

		virtual void ReadAsync(IOBuffer* buf) = 0;

		// set internal mode
		virtual void SetMode(int mode) = 0;

		// error handling

		// return error of stream
		virtual int error() { return m_error; }

	protected:
		DataHandler m_data_handler;
		EventHandler m_end_handler;
		ErrorHandler m_error_handler;
		int m_error;
	};
}

#endif // !PSIO_STREAM_H
