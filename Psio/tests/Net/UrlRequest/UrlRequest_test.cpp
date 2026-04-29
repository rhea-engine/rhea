/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "UrlRequest.h"
#include "UrlResponse.h"
#include "../Socket/Socket_test.h"
#include "../Http/HttpNetworkTransaction.h"

using testing::_;
using testing::MockFunction;
using testing::Return;

namespace psio {

	class UrlRequestT : public UrlRequest
	{
	public:
		UrlRequestT(Engine* engine, HttpTransaction* transaction) :
			UrlRequest(engine)
		{
			m_transaction = transaction;
		}
		void SetTransaction(HttpTransaction* transaction) { m_transaction = transaction; }
	};

	class UrlRequestTest : public ::testing::Test
	{
	public:
		void DoRequest(std::string url, UrlRequest::methods method, UrlRequest::ResponseHandler on_response, UrlRequest::ErrorHandler on_error) {
			if (!request)
				request = new UrlRequestT(&engine, m_transaction);
			request->Request(url, method,
				[&,on_response](UrlResponse* res) {
					on_response(res);
					Finalize();
				},
				[&,on_error](int err) {
					failed = true;
					on_error(err); 
					Finalize();
				});
		}

		void Finalize() {
			responses++;
			finished = true;
			delete request;
			request = nullptr;
		}

		bool finished = false;
		int responses = 0;
		bool success = false;
		bool failed = false;
		UrlRequest* request = nullptr;
		HttpTransaction* m_transaction = nullptr;
		Engine engine;
		UrlRequest::ResponseHandler _on_response;
		UrlRequest::ErrorHandler _on_error;
	};

	TEST_F(UrlRequestTest, OnNetworkError) {
		MockTransaction* transaction;
		transaction = new MockTransaction();
		transaction->setdata(nullptr, -1);
		m_transaction = transaction;

		MockFunction<void(UrlResponse*)> mockResponseCallback;
		MockFunction<void(int)> mockErrorCallback;

		EXPECT_CALL(mockResponseCallback, Call(_)).Times(0);
		EXPECT_CALL(mockErrorCallback, Call(_)).WillOnce(Return());

		DoRequest("http://www.example.org", UrlRequest::methods::REQUEST_GET, mockResponseCallback.AsStdFunction(), mockErrorCallback.AsStdFunction());
	}

	TEST_F(UrlRequestTest, OnResponseSuccessful) {
		
		MockTransaction* transaction;

		char testbuf[] = "test";

		transaction = new MockTransaction();
		transaction->setdata(testbuf, strlen(testbuf));
		m_transaction = transaction;

		MockFunction<void(UrlResponse*)> mockResponseCallback;
		MockFunction<void(int)> mockErrorCallback;

		EXPECT_CALL(mockResponseCallback, Call(_)).WillOnce(Return());
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		DoRequest("http://www.example.org", UrlRequest::methods::REQUEST_GET, mockResponseCallback.AsStdFunction(), mockErrorCallback.AsStdFunction());
	}

	// test if simple get transaction works
	TEST_F(UrlRequestTest, SimpleGet) {
		Engine engine;
		FakeNetwork network(&engine);
		engine.setNetwork(&network);
		FakeSocket* sock = new FakeSocket(&network);
		network.SetSocket(sock);
		m_transaction = new HttpNetworkTransaction(&engine);

		char testbuf[] = "test";

		sock->SetData(testbuf, strlen(testbuf));

		MockFunction<void(UrlResponse*)> mockResponseCallback;
		MockFunction<void(int)> mockErrorCallback;

		EXPECT_CALL(mockResponseCallback, Call(_)).WillOnce(Return());
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		DoRequest("http://www.example.org", UrlRequest::methods::REQUEST_GET, mockResponseCallback.AsStdFunction(), mockErrorCallback.AsStdFunction());
		do {
			sock->Poll(true);
		} while (!finished);
	}

	TEST_F(UrlRequestTest, RealGet) {		
		
		MockFunction<void(UrlResponse*)> mockResponseCallback;
		MockFunction<void(int)> mockErrorCallback;
		
		EXPECT_CALL(mockResponseCallback, Call(_)).Times(1);
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		m_transaction = new HttpNetworkTransaction(&engine);
		DoRequest("http://127.0.0.1/", UrlRequest::methods::REQUEST_GET, mockResponseCallback.AsStdFunction(), mockErrorCallback.AsStdFunction());
		do {
			engine.getNet()->RunLoop();
		} while (responses<1);
	}

	TEST_F(UrlRequestTest, RealGetRedirect) {

		MockFunction<void(UrlResponse*)> mockResponseCallback;
		MockFunction<void(int)> mockErrorCallback;

		EXPECT_CALL(mockResponseCallback, Call(_)).Times(1);
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		m_transaction = nullptr;// new HttpNetworkTransaction(&engine);
		DoRequest("http://127.0.0.1/iris", UrlRequest::methods::REQUEST_GET, mockResponseCallback.AsStdFunction(), mockErrorCallback.AsStdFunction());
		do {
			engine.getNet()->RunLoop();
		} while (responses < 1);
	}

	TEST_F(UrlRequestTest, RealGetTwice) {

		MockFunction<void(UrlResponse*)> mockResponseCallback;
		MockFunction<void(int)> mockErrorCallback;

		EXPECT_CALL(mockResponseCallback, Call(_)).Times(2);
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		m_transaction = new HttpNetworkTransaction(&engine);
		DoRequest("http://127.0.0.1/iris/", UrlRequest::methods::REQUEST_GET, mockResponseCallback.AsStdFunction(), mockErrorCallback.AsStdFunction());
		do {
			engine.getNet()->RunLoop();
		} while (responses < 1);

		m_transaction = new HttpNetworkTransaction(&engine);
		DoRequest("http://127.0.0.1/", UrlRequest::methods::REQUEST_GET, mockResponseCallback.AsStdFunction(), mockErrorCallback.AsStdFunction());
		do {
			engine.getNet()->RunLoop();
		} while (responses < 2);
	}

	TEST_F(UrlRequestTest, LocalSSLGet) {

		MockFunction<void(UrlResponse*)> mockResponseCallback;
		MockFunction<void(int)> mockErrorCallback;

		EXPECT_CALL(mockResponseCallback, Call(_)).Times(1);
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		m_transaction = new HttpNetworkTransaction(&engine);
		DoRequest("https://127.0.0.1:4433/", UrlRequest::methods::REQUEST_GET, mockResponseCallback.AsStdFunction(), mockErrorCallback.AsStdFunction());
		do {
			engine.getNet()->RunLoop();
		} while (responses < 1);
	}

	TEST_F(UrlRequestTest, RealSSLGet) {

		MockFunction<void(UrlResponse*)> mockResponseCallback;
		MockFunction<void(int)> mockErrorCallback;

		EXPECT_CALL(mockResponseCallback, Call(_)).Times(1);
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		m_transaction = new HttpNetworkTransaction(&engine);
		DoRequest("https://www.xelia.org/", UrlRequest::methods::REQUEST_GET, mockResponseCallback.AsStdFunction(), mockErrorCallback.AsStdFunction());
		do {
			engine.getNet()->RunLoop();
		} while (responses < 1);
	}
	
	TEST_F(UrlRequestTest, RealSSLGetRedirect) {

		MockFunction<void(UrlResponse*)> mockResponseCallback;
		MockFunction<void(int)> mockErrorCallback;

		EXPECT_CALL(mockResponseCallback, Call(_)).Times(1);
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		m_transaction = nullptr;// new HttpNetworkTransaction(&engine);
		DoRequest("https://www.xelia.org/iris", UrlRequest::methods::REQUEST_GET, mockResponseCallback.AsStdFunction(), mockErrorCallback.AsStdFunction());
		do {
			engine.getNet()->RunLoop();
		} while (responses < 1);
	}
	
}