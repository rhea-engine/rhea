/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef MWNET_HTTP_RESPONSE
#define MWNET_HTTP_RESPONSE

#include <memory>
#include <unordered_map>
#include <string>

namespace psio
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
		
		// set response code
		void setResponseCode(int code) { m_response_code = code; }
		 
		// write all headers
		void writeHeaders();

		// send error
		int sendError(int error_code);

		// send error with message
		int sendErrorMessage(int error_code, unsigned char* error_message, size_t msglen);

		// send data
		int Write(const char* buf, size_t buflen);

		// get response code
		int getResponseCode() { return m_response_code; }

		// get content length
		size_t getContentLength() { return m_content_len; }

	protected:
		Http* m_stream;
		size_t m_content_len;
		int m_response_code; // HTTP status code
		bool m_headers_sent;
		std::unordered_map<std::string, std::string> m_headers;

		// get status messgae
		unsigned char* getStatusMessage();
	};
}
#endif // !MWNET_HTTP_RESPONSE
