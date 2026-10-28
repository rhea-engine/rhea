#include <Orion/RequestError.h>
#include <Psio/Net/Network.h>

namespace orion
{
	Error translateTransportError(int error)
	{
		switch (error) {
		case psio::Network::Err::OK:
			return Error::OK;
		case psio::Network::Err::IO_PENDING:
			return Error::IO_PENDING;
		case psio::Network::Err::ABORTED:
			return Error::CANCELED;
		case psio::Network::Err::REQUEST_URL_INVALID:
			return Error::CONFIG_ERR;
		case psio::Network::Err::FAILED:
		case psio::Network::Err::NO_SOCKET:
		case psio::Network::Err::NOT_CONNECTED:
		case psio::Network::Err::FAULT:
		case psio::Network::Err::CONN_RESET:
		case psio::Network::Err::INVALID_HTTP_RESPONSE:
		case psio::Network::Err::TOO_MANY_REDIRECTS:
		case psio::Network::Err::RESPONSE_HEADERS_TRUNCATED:
		case psio::Network::Err::RESPONSE_HEADERS_TOO_BIG:
			return Error::TRANSPORT_ERROR;
		default:
			return Error::UNKNOWN_ERROR;
		}
	}

	std::string getErrorMessage(Error err)
	{
		switch (err) {
		case Error::OK:
			return "OK";
		case Error::IO_PENDING:
			return "Data is pending";
		case Error::REALM_NOT_FOUND:
			return "Realm not found";
		case Error::RESOURCE_NOT_FOUND:
			return "Resource not found";
		case Error::NOT_IMPLEMENTED:
			return "Not implemented";
		case Error::NO_ACCESS:
			return "Permission denied";
		case Error::NO_AUTH:
			return "Authentication failed";
		case Error::PATH_NOT_FOUND:
			return "Path not found";
		case Error::CONFIG_ERR:
			return "Invalid request configuration";
		case Error::DATA_INVALID:
			return "Invalid resource data";
		case Error::TRANSPORT_ERROR:
			return "Resource transport failed";
		case Error::CANCELED:
			return "Request canceled";
		case Error::UNKNOWN_ERROR:
			return "Unknown request error";
		default:
			return "General error";
		}
	}
}
