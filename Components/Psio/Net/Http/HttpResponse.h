#ifndef MWNET_HTTP_RESPONSE
#define MWNET_HTTP_RESPONSE

#include <memory>
#include <unordered_map>
#include <string>

namespace Psio
{
	class Http;

	class HttpResponse
	{
	public:
		HttpResponse(Http* stream);

		// end the response
		int end();

		// set a header
		void setHeader(std::string key, std::string value);

		// set content length
		void setContentLength(size_t len) { m_content_len = len; }

		// write all headers
		void writeHeaders();

		// send error
		int sendError(int error_code);
		// send error with message
		int sendErrorMessage(int error_code, unsigned char* error_message);

		// send data
		void Write(const char* buf, size_t buflen);

	protected:
		Http* m_stream;
		size_t m_content_len;
		bool m_headers_sent;
		std::unordered_map<std::string, std::string> m_headers;
	};
}
#endif // !MWNET_HTTP_RESPONSE
