#ifndef ORION_REQUESTERROR_H
#define ORION_REQUESTERROR_H

#include <string>

namespace orion
{
	enum class Error
	{
		OK = 0,
		IO_PENDING = -1,
		REALM_NOT_FOUND = -2,
		RESOURCE_NOT_FOUND = -3,
		NOT_IMPLEMENTED = -4,
		PATH_NOT_FOUND = -5,
		NO_AUTH = -6,
		NO_ACCESS = -7,
		CONFIG_ERR = -8,
		DATA_INVALID = -9,
		TRANSPORT_ERROR = -10,
		CANCELED = -11,
		UNKNOWN_ERROR = -1000
	};

	// Translate Psio network statuses, not HTTP status codes or arbitrary stream errors.
	Error translateTransportError(int error);

	// get an english error message for the error
	std::string getErrorMessage(Error err);
}

#endif // !ORION_REQUESTERROR_H
