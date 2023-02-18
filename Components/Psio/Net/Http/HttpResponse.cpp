#include "HttpResponse.h"
#include "Http.h"

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
	}

	void HttpResponse::setHeader(std::string key, std::string value)
	{
		m_headers[key] = value;
	}

	void HttpResponse::writeHeaders()
	{
		char* status = "HTTP/1.1 200 OK\r\n";
		unsigned char hdr[8192];

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

	void HttpResponse::Write(const char* buf, size_t buflen)
	{
		if (!m_headers_sent)
			writeHeaders();

		m_stream->Write(buf, buflen);
	}

	int HttpResponse::sendError(int error_code)
	{
		unsigned char* error_message;

		switch (error_code) {
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
		m_headers_sent = true;

		return sendErrorMessage(error_code, error_message);
	}

	int HttpResponse::sendErrorMessage(int error_code, unsigned char* error_message)
	{
		unsigned char msg[256];

		snprintf((char*)msg, 255, "HTTP/1.1 %d %s\r\n\r\n", error_code, error_message);
		m_stream->Write((char*)msg, strlen((char*)msg));
		return m_stream->End();
	}

	int HttpResponse::end()
	{
		return m_stream->End();
	}
}