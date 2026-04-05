#include "Response.h"

namespace psio
{
	Response::~Response()
	{
	}

	void Response::setHeader(std::string key, std::string value)
	{
		m_headers[key] = value;
	}

	void Response::write(unsigned char* buf, size_t len)
	{
		if (!m_headers_sent)
			writeHeaders();
		m_conn->write(buf, len);
	}

	void Response::send(unsigned char* buf, size_t len)
	{
		m_content_len = len;
		write(buf, len);
		end();
	}

	void Response::writeHeaders()
	{
		char* status = "200 OK\r\n";

		m_conn->write((unsigned char*)status, strlen(status));
		// end headers

		m_headers_sent = true;
	}

	void Response::sendError(int error_code)
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

		sendErrorMessage(error_code, error_message);
	}

	void Response::sendErrorMessage(int error_code, unsigned char* error_message)
	{
		unsigned char msg[256];

		snprintf((char*)msg, 255, "%d %s\r\n", error_code, error_message);
		m_conn->write(msg, strlen((char*)msg));
		end();
	}

	void Response::end()
	{
		// when repsonse is sent clear the request
		m_request->clear();
		m_request.reset();

		// close the connection
		m_conn->close();
	}
}