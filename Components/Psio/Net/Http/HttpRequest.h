#ifndef PSIO_NET_HTTP_REQUEST_H
#define PSIO_NET_HTTP_REQUEST_H

#include "../UrlRequest/Url.h"
#include "HttpHeaders.h"
#include "Http.h"

namespace Psio
{
	class  HttpRequest
	{
	public:
		HttpRequest() :
			m_http(nullptr)
		{}
		HttpRequest(std::string& url) :
			url(url),
			m_http(nullptr)
		{}
		HttpRequest(Http* http) :
			m_http(http)
		{}

		// the request url
		Url url;

		// the request headers
		HttpHeaders headers;

		void SetHeaders(HttpHeaders* hdrs) { headers = *hdrs; }

	protected:
		Http* m_http;
	};
}
#endif // !PSIO_NET_HTTP_REQUEST_H
