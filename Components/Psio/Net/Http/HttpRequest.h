#ifndef PSIO_NET_HTTP_REQUEST_H
#define PSIO_NET_HTTP_REQUEST_H

#include "../UrlRequest/Url.h"
#include "HttpHeaders.h"

namespace Psio
{
	struct HttpRequest
	{
		HttpRequest() {}
		HttpRequest(std::string& url) :
			url(url)
		{}

		// the request url
		Url url;

		// the request headers
		HttpHeaders headers;
	};
}
#endif // !PSIO_NET_HTTP_REQUEST_H
