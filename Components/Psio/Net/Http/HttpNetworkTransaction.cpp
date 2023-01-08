#include "HttpNetworkTransaction.h"
#include "../Socket/TCPSocket.h"
#include "../Socket/ConnectorFactory.h"
#include <Psio/Log.h>

namespace Psio
{
	HttpNetworkTransaction::~HttpNetworkTransaction()
	{
	}

	int HttpNetworkTransaction::Start(HttpRequest& req, Network::CompletionHandler handler)
	{
		m_state = State::HTTP_NONE;
		m_next_state = State::HTTP_CREATE_STREAM;
		m_request = req;
		m_handler = handler;
		
		int ret = DoLoop(Network::Err::OK);
		
		// this always returns IO_PENDING
		return ret;
	}

	int HttpNetworkTransaction::ReadBody(IOBuffer* buf, size_t chunk_len, Network::CompletionHandler handler)
	{
		if (!m_stream)
			return 0;
		if (m_state == State::HTTP_SEND_REQUEST_COMPLETED)
			return 0;

		m_user_buf = buf;
		m_user_buf_len = chunk_len;
		m_next_state = State::HTTP_READ_BODY;
		m_handler = handler;

		int res = DoLoop(0);
		return res;
	}

	HttpHeaders* HttpNetworkTransaction::GetHeaders()
	{
		if (m_stream)
			return m_stream->GetHeaders();
		return nullptr;
	}

	int HttpNetworkTransaction::DoLoop(int result)
	{
		int m_error = result;

		do
		{
			m_state = m_next_state;
			m_next_state = State::HTTP_NONE;

			switch (m_state)
			{
			case State::HTTP_NONE:
				break;
			case State::HTTP_CREATE_STREAM:
				m_error = DoCreateStream(m_error);
				break;
			case State::HTTP_CREATE_STREAM_COMPLETE:
				m_error = DoCreateStreamComplete(m_error);
				break;
			case State::HTTP_BUILD_HEADERS:
				m_error = DoBuildHeaders(m_error);
				break;
			case State::HTTP_BUILD_HEADERS_COMPLETED:
				m_error = DoBuildHeadersComplete(m_error);
				break;
			case State::HTTP_SEND_REQUEST:
				m_error = DoSendRequest(m_error);
				break;
			case State::HTTP_SEND_REQUEST_COMPLETED:
				m_error = DoSendRequestComplete(m_error);
				break;
			case State::HTTP_READ_HEADERS:
				m_error = DoReadHeaders(m_error);
				break;
			case State::HTTP_READ_HEADERS_COMPLETED:
				m_error = DoReadHeadersComplete(m_error);
				break;
			case State::HTTP_READ_BODY:
				m_error = DoReadBody(m_error);
				break;
			case State::HTTP_READ_BODY_COMPLETED:
				m_error = DoReadBodyComplete(m_error);
				break;
			case State::HTTP_REQUEST_COMPLETED:
				m_error = DoRequestComplete(m_error);
				break;
			default:
				break;
			}
				
		} while ((m_error != Network::Err::IO_PENDING) && (m_state != State::HTTP_NONE));

		return m_error;
	}

	int HttpNetworkTransaction::DoCreateStream(int result)
	{
		m_next_state = State::HTTP_CREATE_STREAM_COMPLETE;
		std::string host = m_request.headers["Host"];
		unsigned short port = m_request.url.Port();
		
		// check if scheme is defined
		std::string scheme = m_request.url.Scheme();

		m_connector = ConnectorFactory::factory().Create(m_engine->getNet(), scheme, host, port);

		int ret = m_connector->Start([&](int result) -> void {
			CallDoLoop(result);
			});

		return ret;
	}

	int HttpNetworkTransaction::DoCreateStreamComplete(int result)
	{
		// stream established
		if (result == Network::Err::OK) {
			m_next_state = State::HTTP_BUILD_HEADERS;
			m_stream = std::make_unique<HttpStream>(m_connector->GetSocket());
			// TODO: Do we have handler for connection established?
			//			m_handler(result);
			return Network::Err::OK;
		}
		else if (result == Network::Err::IO_PENDING) {
			m_next_state = State::HTTP_CREATE_STREAM_COMPLETE;
			return result;
		}
		else {
			// on connect error call the handler with the error and set the state to NONE
			m_state = State::HTTP_NONE;
			m_next_state = State::HTTP_NONE;
			return result;
		}
	}

	int HttpNetworkTransaction::DoBuildHeaders(int result)
	{
		if (result == Network::Err::OK) {
			m_next_state = State::HTTP_BUILD_HEADERS_COMPLETED;
		}
		return result;
	}

	int HttpNetworkTransaction::DoBuildHeadersComplete(int result)
	{
		if (result == Network::Err::OK) {
			m_next_state = State::HTTP_SEND_REQUEST;
		}
		return result;
	}

	int HttpNetworkTransaction::DoSendRequest(int result)
	{
		if (result == Network::Err::OK) {
			m_next_state = State::HTTP_SEND_REQUEST_COMPLETED;
			return m_stream->SendRequest(m_request, [&](int err) {
				CallDoLoop(err);
				});
		}
		return result;
	}

	int HttpNetworkTransaction::DoSendRequestComplete(int result)
	{
		if (result == Network::Err::OK) {
			m_next_state = State::HTTP_READ_HEADERS;
		}
		return result;
	}

	int HttpNetworkTransaction::DoReadHeaders(int result)
	{
		if (result == Network::Err::OK) {
			m_next_state = State::HTTP_READ_HEADERS_COMPLETED;
			return m_stream->ReadResponseHeaders([&](int result) {
				CallDoLoop(result);
				});
		}
		return result;
	}

	int HttpNetworkTransaction::DoReadHeadersComplete(int result)
	{
		// end of the start part of the state machine
		return result;
	}

	int HttpNetworkTransaction::DoReadBody(int result)
	{
		if (result == Network::Err::OK) {
			m_next_state = State::HTTP_READ_BODY_COMPLETED;
			return m_stream->ReadBody(m_user_buf, m_user_buf_len, [&](int result) {
				CallDoLoop(result);
				});
		}
		return result;
	}

	int HttpNetworkTransaction::DoReadBodyComplete(int result)
	{
		if (result == Network::Err::OK) {
			m_next_state = State::HTTP_REQUEST_COMPLETED;
		}
		return result;
	}

	int HttpNetworkTransaction::DoRequestComplete(int result)
	{
		if (result == Network::Err::OK) {
			m_next_state = State::HTTP_NONE;
		}
		return result;
	}
	
	void HttpNetworkTransaction::CallDoLoop(int result)
	{
		int res = DoLoop(result);
		if ((res != Network::Err::IO_PENDING) && (m_handler != NULL))
			m_handler(res);

	}
}