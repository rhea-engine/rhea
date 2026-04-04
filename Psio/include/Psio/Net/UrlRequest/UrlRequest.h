#ifndef PSIO_NET_URL_REQUEST_H
#define PSIO_NET_URL_REQUEST_H

#include "../Http/HttpRequest.h"
#include "UrlResponse.h"

#include <functional>

namespace psio
{
	class Engine;
	class HttpTransaction;

	class UrlRequest
	{
	public:
		UrlRequest(Engine* engine) :
			m_engine(engine),			
			m_transaction(nullptr),
			m_response(nullptr),
			m_on_error_handler(nullptr),
			m_nof_redirects(0)
		{}
		~UrlRequest();

		enum class methods {
			REQUEST_GET = 1,
			REQUEST_POST = 2
		};

		// callbacks
		typedef std::function<void(UrlResponse*)> ResponseHandler;
		typedef std::function<void(int)> ErrorHandler;
		
		// called when the response is fully read
		void OnResponse(ResponseHandler handler) { m_on_response_handler = handler; }
		// called on error
		void OnError(ErrorHandler handler) { m_on_error_handler = handler; }

		// call a Request on url with the given HTTP method
		void Request(std::string url, methods method);
		// call a Request on url with the given HTTP method - set the response handlers directly
		void Request(std::string url, methods method, ResponseHandler on_response, ErrorHandler on_error);

		void Get(std::string url);
		void Get(std::string url, ResponseHandler on_response, ErrorHandler on_error);

	protected:
		Engine* m_engine;
		HttpRequest m_request;
		ResponseHandler m_on_response_handler;
		ErrorHandler m_on_error_handler;

		HttpTransaction* m_transaction;		
		UrlResponse* m_response;

		// called when the transaction is started
		void OnTransaction(int result, methods method);

		int m_nof_redirects;
	};
}

#endif // !PSIO_NET_URL_REQUEST_H
