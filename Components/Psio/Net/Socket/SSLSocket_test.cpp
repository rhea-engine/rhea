#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include "SSLSocket.h"
#include "TCPSocket.h"
#include "../Network.h"
#include "SocketAddress.h"

namespace Psio
{
	class SSLSocketTest : public ::testing::Test {
	public:
		SSLSocketTest()
		{
			net = new Network(nullptr);
			net->Init();
		}
		~SSLSocketTest() {
			if (sock)
				delete sock;
		}

	protected:
		Network* net;
		SSLSocket* sock;
		std::unique_ptr<TCPSocket> tp_sock;
		bool finished = false;

		int SendRequest(Psio::IOBuffer* buf, Psio::Socket::CompletionHandler handler) {
			return sock->Write(buf, [&](int err) {
				handler(err);
				});
		}

		int ReadResponse(Psio::IOBuffer* buf, Psio::Socket::CompletionHandler handler) {
			return sock->Read(buf, [&](int err) {
				handler(err);
				});
		}

	};

	TEST_F(SSLSocketTest, Connect) {
		tp_sock = std::make_unique<TCPSocket>(TCPSocket(net));
		sock = new SSLSocket(net, std::move(tp_sock), nullptr);
		SocketAddress addr;
		addr.SetHost("127.0.0.1", 4433);
		int res;

		sock->Connect(addr, false, [&](int err) {
			if (err < 0)
				res = err;
			else
				res = 0;
			finished = true;
		});

		do {
			net->RunLoop();
		} while (!finished);

		EXPECT_EQ(res, 0);
	}

	TEST_F(SSLSocketTest, ConnectLocalReadTest) {
		tp_sock = std::make_unique<TCPSocket>(TCPSocket(net));
		sock = new SSLSocket(net, std::move(tp_sock), nullptr);
		SocketAddress addr;
		addr.SetHost("127.0.0.1", 4433);
		int res;
		int res_read;
		IOBuffer buf(2048);
		char sendbuf[] = "GET / HTTP/1.1\r\n"
			"Host: www.meworla.com\r\n"
			"User-Agent: TestMock v1\r\n\r\n";
		IOBuffer outbuf(2048);
		outbuf.Write(sendbuf, strlen(sendbuf));

		sock->Connect(addr, false, [&](int err) {
			if (err < 0) {
				res = err;
				finished = true;
			}
			else {
				res = 0;
				int ret = SendRequest(&outbuf, [&](int err) {
					if (err < 0)
						res = err;
					int ret = ReadResponse(&buf, [&](int err) {
						if (err < 0)
							res_read = err;
						else {
							res_read = 0;
						}
						finished = true;
						});

					});

				if (ret > 0) {
					res = 0;
					ret = ReadResponse(&buf, [&](int err) {
						if (err < 0)
							res_read = err;
						else {
							res_read = 0;
						}
						finished = true;
						});
					if (ret > 0) {
						res_read = 0;
						finished = true;
					}
					else if (ret != Psio::Network::IO_PENDING) {
						res_read = ret;
						finished = true;
					}
				}
				else if (ret != Psio::Network::IO_PENDING) {
					res = ret;
					res_read = 0;
					finished = true;
				}
			}
			});

		do {
			net->RunLoop();
		} while (!finished);

		EXPECT_EQ(res, 0);
		EXPECT_EQ(res_read, 0);
	}

/*
	TEST_F(SSLSocketTest, GetMeworla) {
		tp_sock = std::make_unique<TCPSocket>(TCPSocket(net));
		sock = new SSLSocket(net, std::move(tp_sock), nullptr);
		SocketAddress addr;
		addr.SetHost("www.meworla.com", 443);
//		addr.SetHost("127.0.0.1", 4433);
		int res;
		int res_read;
		IOBuffer buf(2048);
		char sendbuf[] = "GET / HTTP/1.1\r\n"
			"Host: www.meworla.com\r\n"
			"User-Agent: TestMock v1\r\n\r\n";
		IOBuffer outbuf(2048);
		outbuf.Write(sendbuf, strlen(sendbuf));

		sock->Connect(addr, false, [&](int err) {
			if (err < 0)
				res = err;
			else {
				res = 0;
				sock->Write(&outbuf, [&](int err) {
					if (err < 0)
						res = err;
					else {
						sock->Read(&buf, [&](int err) {
							if (err < 0)
								res_read = err;
							else {
								res_read = 0;								
							}
							finished = true;
							});
					}
					});

			}
			});

		do {
			net->RunLoop();
		} while (!finished);

		EXPECT_EQ(res, 0);
		EXPECT_EQ(res_read, 0);
	}
	*/
}