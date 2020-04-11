#include "HttpResponse.h"
#include "Connection.h"
#include "Request.h"

namespace Hermes
{
	HttpResponse::HttpResponse(Connection* conn, std::shared_ptr<Request> request) : Response(conn, request)
	{
		// set default headers
		m_headers["Server"] = "Hermes/1.0";
		m_headers["Content-type"] = "text/plain";
	}

	void HttpResponse::writeHeaders()
	{
		char* status = "HTTP/1.1 200 OK\r\n";
		unsigned char hdr[8192];

		// write status line
		m_conn->write((unsigned char*)status, strlen(status));

		if (m_content_len>0)
			m_headers["Content-length"] = std::to_string(m_content_len);
		
		// send headers
		for (std::pair<std::string, std::string> header : m_headers)
		{
			snprintf((char*)hdr, 8192, "%s: %s\r\n", header.first.c_str(), header.second.c_str());
			m_conn->write(hdr, strlen((char*)hdr));
		}

		// end headers
		m_conn->write((unsigned char*)"\r\n", strlen("\r\n"));
	}

	void HttpResponse::sendErrorMessage(int error_code, unsigned char* error_message)
	{
		unsigned char msg[256];

		snprintf((char*)msg, 255, "HTTP/1.1 %d %s\r\n\r\n", error_code, error_message);
		m_conn->write(msg, strlen((char*)msg));
		end();
	}
}