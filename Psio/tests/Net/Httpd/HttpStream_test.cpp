/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cstring>

#include "../Socket/Socket_test.h"
#include "HttpStream.h"
#include "HttpRequest.h"
#include <algorithm>
#undef min;

using testing::_;
using testing::MockFunction;
using testing::Return;

#define CHUNK_LEN 2048*1024

namespace psio
{

	class HttpStreamTest : public ::testing::Test {
	public:
	protected:
		void SetUp() override
		{
			network = new FakeNetwork(&engine);
			engine.setNetwork(network);
			sock = new FakeSocket(network);
			static_cast<FakeNetwork*>(network)->SetSocket(sock);
			buf = new IOBuffer(CHUNK_LEN);			
		}

		void TearDown() override {
			delete stream;
			delete network;
			delete buf;
		}

		void SendRequest(std::string& url, HttpHeaders* headers) {
			request.url = url;
			if (headers) {
				request.headers = *headers;
			}
			if (!stream) {
				realsock = std::make_unique<TCPSocket>(network);
				SocketAddress m_addr;
				m_addr.SetHost(request.url.Hostname(), request.url.Port());
				int ret = realsock->Connect(m_addr, false, [&](int error) -> void {
					stream = new HttpStream(std::move(realsock));
					int ret = stream->SendRequest(request, [&](int result) {
						OnRequestSent(result);
						});
					if (ret >= 0)
						OnRequestSent(ret);
					else if (ret < -1) {
						printf("ConnectCB net err: %d\n", ret);
						finished = true;
						req_failed = true;
					}
					return;
					});
				if (ret == -1)
					return;
				else
					stream = new HttpStream(std::move(realsock));
			}			
			int ret = stream->SendRequest(request, [&](int result) {
				OnRequestSent(result);
				});
			if (ret >= 0)
				OnRequestSent(ret);
			else if (ret < -1) {
				printf("Connect net err: %d\n", ret);
				finished = true;
				req_failed = true;
			}

		}

		void OnRequestSent(int result)
		{
			if (result < 0) {
				req_failed = true;
				mockErrorCallback.Call(result);
				finished = true;
			}
			else {
				//  read headers
				int ret = stream->ReadResponseHeaders([&](int result) {
					OnHeaders(result);
					});
				if (ret != -1)
					OnHeaders(ret);
			}
		}

		void OnHeaders(int result) {
			if (result < 0) {
				req_failed = true;
				mockErrorCallback.Call(result);
				finished = true;
			}
			else {
				res_headers = stream->GetHeaders();
				if (res_headers) {
					has_headers = true;
					if (res_headers->GetResponseCode() == 301) {
						// we are not interested in the response body - drain it
						int ret = stream->DrainBody([&](int result) {
							OnBody(result);
							});
						if (ret != -1) {
							// synchronous answer
							OnBody(result);
						}
					}
				}
				int ret = stream->ReadBody(buf, CHUNK_LEN, [&](int result) {
					OnBody(result);
					});
				if (ret != -1) {
					// synchronous answer
					OnBody(result);
				}
			}

		}

		void OnBody(int result) {
			if (result < 0)
				req_failed = true;
			if (!buf->Remaining())
				empty_body = true;
			mockResponseCallback.Call(result);
			finished = true;
		}

		void Loop(bool keep_alive = true) {
			do {
				sock->Poll(keep_alive);
			} while (!finished);
		}

	protected:
		Engine engine;
		Network* network = nullptr;
		FakeSocket *sock = nullptr;
		std::unique_ptr<FakeSocket> f_sock;
		std::unique_ptr<TCPSocket> realsock;
		HttpStream *stream = nullptr;
		IOBuffer* buf = nullptr;
		IOBuffer* body = nullptr;
		HttpRequest request;
		HttpHeaders* res_headers = nullptr;

		bool finished = false;

		bool req_failed = false;
		bool empty_body = false;
		bool has_headers = false;

		MockFunction<void(int)> mockErrorCallback;
		MockFunction<void(int)> mockResponseCallback;
	};

	// tests if a simple GET with no headers sent by client returns the given body
	TEST_F(HttpStreamTest, SimpleGetNoHeaders) {

		f_sock.reset(sock);
		stream = new HttpStream(std::move(f_sock));

		char header[] = "HTTP/1.1 200 OK\r\n\r\n";
		sock->SetData(header, strlen(header));
		char testbuf[] = "test";
		sock->SetData(testbuf, strlen(testbuf));

		EXPECT_CALL(mockResponseCallback, Call(_)).WillOnce(Return());
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		std::string url = "/";
		SendRequest(url, nullptr);
		Loop();

		EXPECT_FALSE(req_failed) << "Request failed";
		EXPECT_FALSE(empty_body);
		size_t len = std::min(buf->Remaining(), strlen(testbuf));
		EXPECT_TRUE(has_headers) << "No headers read";
		EXPECT_TRUE(res_headers && res_headers->GetMajor() == 1 && res_headers->GetMinor() == 1) << "Header version missmatch - should be 1.1";
		EXPECT_TRUE(0 == std::memcmp(testbuf, buf->reader(), len));
	}

	// tests if a simple GET with no headers sent by client returns the given body and the http version 0.9 is parsed correctly
	TEST_F(HttpStreamTest, SimpleGetNoHeadersHttp09) {
		
		f_sock.reset(sock);
		stream = new HttpStream(std::move(f_sock));

		char testbuf[] = "test";
		sock->SetData(testbuf, strlen(testbuf));

		EXPECT_CALL(mockResponseCallback, Call(_)).WillOnce(Return());
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		std::string url = "/";
		SendRequest(url, nullptr);
		Loop(false);

		EXPECT_FALSE(req_failed);
		EXPECT_FALSE(empty_body);
		EXPECT_TRUE(has_headers) << "No headers read";
		EXPECT_TRUE(res_headers && res_headers->GetMajor() == 0 && res_headers->GetMinor() == 9) << "Header version missmatch - should be 0.9";;
		size_t len = std::min(buf->Remaining(), strlen(testbuf));
		EXPECT_TRUE(0 == std::memcmp(testbuf, buf->reader(), len));
	}

	// test if headers are parsed corretcly
	TEST_F(HttpStreamTest, SimpleGetHeadersParsed) {

		f_sock.reset(sock);
		stream = new HttpStream(std::move(f_sock));

		char header[] = "HTTP/1.1 200 OK\r\n"
			"Server: TestMockServer v1\r\n"
			"Vary: Accept-Encoding\r\n"
			"Content-Length: 123\r\n"
			"Content-Type: text/html;charset=UTF-8\r\n\r\n";
		sock->SetData(header, strlen(header));
		char testbuf[] = "test";
		sock->SetData(testbuf, strlen(testbuf));

		EXPECT_CALL(mockResponseCallback, Call(_)).WillOnce(Return());
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		std::string url = "http://www.example.org";
		SendRequest(url, nullptr);
		Loop();

		EXPECT_FALSE(req_failed) << "Request failed";
		EXPECT_FALSE(empty_body);
		size_t len = std::min(buf->Remaining(), strlen(testbuf));
		EXPECT_TRUE(has_headers) << "No headers read";
		EXPECT_TRUE(res_headers && res_headers->GetMajor() == 1 && res_headers->GetMinor() == 1) << "Header version missmatch - should be 1.1";
		EXPECT_TRUE(res_headers && !(*res_headers)["Server"].compare("TestMockServer v1") && !(*res_headers)["Content-Length"].compare("123")) << "Headers not parsed correctly";
		EXPECT_TRUE(0 == std::memcmp(testbuf, buf->reader(), len));
	}

	// test if correct status line is generated by client
	TEST_F(HttpStreamTest, SimpleGetStatusLineNoHeaders) {

		f_sock.reset(sock);
		stream = new HttpStream(std::move(f_sock));

		char header[] = "HTTP/1.1 200 OK\r\n\r\n";
		sock->SetData(header, strlen(header));
		char testbuf[] = "test";
		sock->SetData(testbuf, strlen(testbuf));

		char sentbuf[] = "GET / HTTP/1.1\r\n\r\n";

		EXPECT_CALL(mockResponseCallback, Call(_)).WillOnce(Return());
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		std::string url = "http://www.example.org/";
		SendRequest(url, nullptr);
		Loop();

		EXPECT_FALSE(req_failed) << "Request failed";
		EXPECT_FALSE(empty_body);
		size_t len = std::min(sock->GetSentData()->Remaining(), strlen(sentbuf));
		EXPECT_TRUE(0 == std::memcmp(sentbuf, sock->GetSentData()->reader(), len));
	}

	// test if correct status line and headers are generated by client
	TEST_F(HttpStreamTest, SimpleGetStatusLineAndHeaders) {

		f_sock.reset(sock);
		stream = new HttpStream(std::move(f_sock));

		char header[] = "HTTP/1.1 200 OK\r\n\r\n";
		sock->SetData(header, strlen(header));
		char testbuf[] = "test";
		sock->SetData(testbuf, strlen(testbuf));

		HttpHeaders hdr;
		hdr.SetHeader("Host", "www.example.org");
		hdr.SetHeader("User-Agent", "TestMock v1");

		char sentbuf[] = "GET / HTTP/1.1\r\n"
			"Host: www.example.org\r\n"
			"User-Agent: TestMock v1\r\n\r\n";

		EXPECT_CALL(mockResponseCallback, Call(_)).WillOnce(Return());
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		std::string url = "http://www.example.org/";
		SendRequest(url, &hdr);
		Loop();

		EXPECT_FALSE(req_failed) << "Request failed";
		EXPECT_FALSE(empty_body);
		size_t len = std::min(sock->GetSentData()->Remaining(), strlen(sentbuf));
		EXPECT_TRUE(0 == std::memcmp(sentbuf, sock->GetSentData()->reader(), len)) << "Sent headers and status line mismatch";
	}

	// drain after non 200-request 
	TEST_F(HttpStreamTest, DrainTwoGets) {

		f_sock.reset(sock);
		stream = new HttpStream(std::move(f_sock));

		char header[] = "HTTP/1.1 301 Moved Permanently\r\n"
			"Server: TestMockServer v1\r\n"
			"Location: http://localhost/path\r\n"
			"Content-Length: 5\r\n\r\n";
		sock->SetData(header, strlen(header));
		char testbuf[] = "test\n";
		sock->SetData(testbuf, strlen(testbuf));

		EXPECT_CALL(mockResponseCallback, Call(_)).Times(2);
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		std::string url = "http://www.example.org/";
		SendRequest(url, nullptr);
		Loop();
		
		EXPECT_FALSE(req_failed) << "Request failed";
		EXPECT_TRUE(empty_body) << "Body was not drained";
		EXPECT_TRUE(has_headers) << "No headers read";
		EXPECT_TRUE(res_headers && res_headers->GetMajor() == 1 && res_headers->GetMinor() == 1) << "Header version missmatch - should be 1.1";
	}

	// Test a real server
	TEST_F(HttpStreamTest, GetFromLocalhost) {

		// override with real network
		delete sock;
		sock = nullptr;
		delete network;
		network = new Network(&engine);
		char answer[] = "HTTP/1.1 200 OK\r\n\r\nsomebody\n";
		bool written = false;

		// a test server
		TCPSocket srv_sock(network);
		int res = srv_sock.Server("0.0.0.0", 8080, false);
		IOBuffer r_buffer(1024);
		IOBuffer w_buffer(1024);
		w_buffer.Write(answer, strlen(answer));
		std::string srv_got;
		int ret = srv_sock.Accept([&](StreamSocket* new_sock, int err) {
			int ret = new_sock->Read(&r_buffer, [&,new_sock](int result)->int {
				srv_got = std::string(r_buffer.reader(), result);
				printf("Server received:\n%s\n--\n", srv_got.c_str());
				int ret = new_sock->Write(&w_buffer, [&](int)->int {
					written = true;
					delete new_sock;
					return 0;
					});
				if (ret != -1) {
					delete new_sock;
					written = true;
				}
				return (result);
				});
			if (ret > 0) {
				written = true;
				delete new_sock;
			}
			return ret;
			});
	
		EXPECT_CALL(mockResponseCallback, Call(_)).Times(1);
		EXPECT_CALL(mockErrorCallback, Call(_)).Times(0);

		std::string url = "http://localhost:80/";
		HttpHeaders headers;
		headers["Host"] = "localhost";
		SendRequest(url, &headers);
		do {
			network->RunLoop();
		} while (!finished);

		EXPECT_FALSE(req_failed) << "Request failed";
		EXPECT_FALSE(empty_body) << "No Body was received";
		EXPECT_TRUE(has_headers) << "No headers read";
		EXPECT_TRUE(res_headers && res_headers->GetMajor() == 1 && res_headers->GetMinor() == 1) << "Header version missmatch - should be 1.1";
	}
}