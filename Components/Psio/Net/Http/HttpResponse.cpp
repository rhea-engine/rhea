#include "HttpResponse.h"
#include "Http.h"
#include <cstring>

namespace Psio
{
	HttpResponse::HttpResponse(Http* stream) :
		m_headers_sent(false),
		m_content_len(0),
		m_stream(stream)
	{
		// set default headers
		m_headers["Server"] = "Hermes/1.0";
		m_headers["Content-type"] = "text/plain";
		m_response_code = 200;
	}

	void HttpResponse::setHeader(std::string key, std::string value)
	{
		m_headers[key] = value;
	}

	void HttpResponse::writeHeaders()
	{
		char status[128];
		unsigned char hdr[8192];

		sprintf(status, "HTTP/1.1 %d %s\r\n", m_response_code, getStatusMessage());

		// write status line
		m_stream->Write(status, strlen(status), false);

		if (m_content_len>0)
			m_headers["Content-length"] = std::to_string(m_content_len);
		
		// send headers
		for (std::pair<std::string, std::string> header : m_headers)
		{
			snprintf((char*)hdr, 8192, "%s: %s\r\n", header.first.c_str(), header.second.c_str());
			m_stream->Write((char*)hdr, strlen((char*)hdr), false);
		}

		// end headers
		m_stream->Write("\r\n", strlen("\r\n"));
		m_headers_sent = true;
	}

	int HttpResponse::Write(const char* buf, size_t buflen)
	{
		if (!m_headers_sent)
			writeHeaders();

		return m_stream->Write(buf, buflen);
	}

	int HttpResponse::sendError(int error_code)
	{
		unsigned char* error_message;

		m_response_code = error_code;
		error_message = getStatusMessage();

		return sendErrorMessage(error_code, error_message, strlen((const char*)error_message));
	}

	unsigned char* HttpResponse::getStatusMessage()
	{
		unsigned char* error_message;

		switch (m_response_code) {
		case 200:
			error_message = (unsigned char*)"OK";
			break;
		case 400:
			error_message = (unsigned char*)"Bad Request";
			break;
		case 404:
			error_message = (unsigned char*)"Not Found";
			break;
		case 405:
			error_message = (unsigned char*)"Method Not Allowd";
			break;
		case 500:
			error_message = (unsigned char*)"Internal Server Error";
			break;
		case 501:
			error_message = (unsigned char*)"Not Implemented";
			break;
		default:
			error_message = (unsigned char*)"Request could not be handled";
		}
		return error_message;
	}

	int HttpResponse::sendErrorMessage(int error_code, unsigned char* error_message, size_t msglen)
	{
		m_response_code = error_code;
		setContentLength(msglen);
		Write((const char*)error_message, msglen);
		return end();
	}

	int HttpResponse::end()
	{
		return m_stream->ResponseEnd();
	}
}