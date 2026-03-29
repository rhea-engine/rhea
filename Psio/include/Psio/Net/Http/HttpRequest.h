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

		// callback that receives data
		typedef std::function<void(Psio::IOBuffer*)> DataCallback;
		// callback that ends the request
		typedef std::function<void(Psio::IOBuffer*)> RequestCompletedCallback;

		// the request url
		Url url;

		// the request headers
		HttpHeaders headers;

		void SetHeaders(HttpHeaders* hdrs) { headers = *hdrs; }

		void OnData(DataCallback callback) { m_on_data = callback; }
		void OnEnd(RequestCompletedCallback callback) { m_on_request_completed = callback; }

		void SendData(Psio::IOBuffer* buffer) {
			if (m_on_data)
				m_on_data(buffer);
		}

		void End(Psio::IOBuffer* buffer) {
			if (m_on_request_completed)
				m_on_request_completed(buffer);
		}

	protected:
		Http* m_http;
		DataCallback m_on_data;
		RequestCompletedCallback m_on_request_completed;
	};
}
#endif // !PSIO_NET_HTTP_REQUEST_H
