#include <Psio/Net/UrlRequest/UrlRequest.h>
#include <Psio/Net/Http/HttpNetworkTransaction.h>
#include <Psio/Log.h>

namespace Psio
{
	UrlRequest::~UrlRequest()
	{
		if (m_transaction) {
			delete m_transaction;
		}
		if (m_response)
			delete m_response;
	}

	void UrlRequest::Request(std::string url, methods method)
	{
		// safety check
		// TODO this is not very smart - we should inform about the error or silently ignore it (perhaps we dont care about errors so the err handler could be empty)
		if (!m_on_response_handler) {
			Log::logger().printf(LOG_ERROR, "UrlRequest: m_on_response_handler is empty");
		}
		if (!m_on_error_handler) {
			Log::logger().printf(LOG_ERROR, "UrlRequest: m_on_error_handler is empty");
		}
		if (!m_on_response_handler || !m_on_error_handler)
			return;

		// create a http transaction
		if (!m_transaction)
			m_transaction = new HttpNetworkTransaction(m_engine);

		Url _url(url);
		if (!_url.IsValid()) {
			m_on_error_handler(Network::Err::REQUEST_URL_INVALID);
			return;
		}

		m_request.url = _url;
		m_request.headers["Host"] = _url.Hostname();

		LOG(LOG_INFO) << "UrlRequest: Get URL " << url;

		int res = m_transaction->Start(m_request, [&, method](int err) {
			OnTransaction(err, method);
			});
		if (res != -1)
			abort();
	}

	void UrlRequest::OnTransaction(int result, methods method)
	{
		if (result >= 0) {

			// read headers
			HttpHeaders* headers = m_transaction->GetHeaders();

			int response_code = 200;
			if (headers) {
				response_code = headers->GetResponseCode();
				DLOG(LOG_DEBUG) << "UrlRequest::OnTransaction: " << response_code << " Length: " << (*headers)["Content-Length"];
			}
			else
				DLOG(LOG_DEBUG) << "UrlRequest::OnTransaction: Received no Header";

			// check if redirect requested
			if (response_code == 301 || response_code == 302 ||
				response_code == 307 || response_code == 308 ||
				response_code == 303)
			{
				std::string new_loc = (*headers)["Location"];
				
				LOG(LOG_INFO) << "Redirect to " << new_loc << " required";

				if (!new_loc.empty()) {
					// only allow 5 redirects
					if (++m_nof_redirects <= 5) {
						// for '303 See Other' we  MUST use GET
						if (headers->GetResponseCode() == 303)
							method = methods::REQUEST_GET;

						// remove the old transaction
						delete m_transaction;
						m_transaction = nullptr;

						Request(new_loc, method);
						return;
					}
					else {
						m_on_error_handler(Network::Err::TOO_MANY_REDIRECTS);
						return;
					}
				}
			}			

			// answer is the actual response
			// setup UrlResponse with a stream of the transaction
			m_response = new UrlResponse(m_transaction, response_code);
			//  call handler so it can setup listeners
			m_on_response_handler(m_response);
		}
		else {
			m_on_error_handler(result);
		}
	}

	void UrlRequest::Request(std::string url, methods method, ResponseHandler on_response, ErrorHandler on_error)
	{
		m_on_response_handler = on_response;
		m_on_error_handler = on_error;
		Request(url, method);
	}

	void UrlRequest::Get(std::string url)
	{
		Request(url, methods::REQUEST_GET);
	}

	void UrlRequest::Get(std::string url, ResponseHandler on_response, ErrorHandler on_error)
	{
		Request(url, methods::REQUEST_GET, on_response, on_error);
	}
}