/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#ifndef PSIO_HTTP_NETWORK_TRANSACTION_H
#define PSIO_HTTP_NETWORK_TRANSACTION_H

#include "HttpTransaction.h"
#include "../Socket/SocketAddress.h"
#include "../IOBuffer.h"
#include "../Socket/Connector.h"
#include "HttpStream.h"

namespace psio
{
	class HttpHeaders;

	class HttpNetworkTransaction : public HttpTransaction
	{
	public:
		HttpNetworkTransaction(Engine *engine) :
			m_engine(engine),
			m_stream(nullptr),
			m_user_buf(nullptr),
			m_user_buf_len(0),
			m_state(State::HTTP_NONE),
			m_next_state(State::HTTP_NONE)
		{}
		~HttpNetworkTransaction();

		int Start(HttpRequest& req, Network::CompletionHandler handler) override;
		int ReadBody(IOBuffer* buf, size_t chunk_len, Network::CompletionHandler handler) override;

		HttpHeaders* GetHeaders();

	protected:
		
		// States for state machine
		enum class State {
			HTTP_NONE,
			HTTP_CREATE_STREAM,
			HTTP_CREATE_STREAM_COMPLETE,
			HTTP_BUILD_HEADERS,
			HTTP_BUILD_HEADERS_COMPLETED,
			HTTP_SEND_REQUEST,
			HTTP_SEND_REQUEST_COMPLETED,
			HTTP_READ_HEADERS,
			HTTP_READ_HEADERS_COMPLETED,
			HTTP_READ_BODY,
			HTTP_READ_BODY_COMPLETED,
			HTTP_REQUEST_COMPLETED
		};

		// state machine
		int DoLoop(int result);

		// state functions
		int DoCreateStream(int result);
		int DoCreateStreamComplete(int result);
		int DoBuildHeaders(int result);
		int DoBuildHeadersComplete(int result);
		int DoSendRequest(int result);
		int DoSendRequestComplete(int result);
		int DoReadHeaders(int result);
		int DoReadHeadersComplete(int result);
		int DoReadBody(int result);
		int DoReadBodyComplete(int result);
		int DoRequestComplete(int result);
		void CallDoLoop(int result);

		State m_state;
		State m_next_state;

		std::unique_ptr<HttpStream> m_stream;
		std::unique_ptr<Connector> m_connector;

		HttpRequest m_request;
		SocketAddress m_s_addr;

		Network::CompletionHandler m_handler;

		IOBuffer* m_user_buf;
		size_t m_user_buf_len;

		Engine* m_engine;
	};
}

#endif // !PSIO_HTTP_NETWORK_TRANSACTION_H
