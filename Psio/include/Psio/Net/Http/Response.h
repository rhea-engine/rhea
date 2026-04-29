/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_RESPONSE_H
#define PSIO_RESPONSE_H

#include <unordered_map>
#include <string>
#include <memory>

namespace psio
{
	class Response
	{
	public:
		Response() :
//			m_conn(conn),
//			m_request(request),
			m_headers_sent(false),
			m_content_len(0)
		{}
		~Response();

		// set a header
		void setHeader(std::string key, std::string value);
		// stream message to output buffer
		void write(unsigned char* buf, size_t len);
		// write buffer to output and end response
		void send(unsigned char* buf, size_t len);
		// write all headers
		virtual void writeHeaders();
		// send error
		virtual void sendError(int error_code);
		// send error with message
		virtual void sendErrorMessage(int error_code, unsigned char* error_message);
		// end the response
		void end();
	protected:
	//	Connection* m_conn;
	//	std::shared_ptr<Request> m_request;
		bool m_headers_sent;
		size_t m_content_len;
		std::unordered_map<std::string, std::string> m_headers;
	};
}
#endif // !PSIO_RESPONSE_H
