#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>
#include "TCPSocket.h"
#include "../Network.h"

// testing tcp socket
// run a lokal server from the test
namespace Psio
{
	class TCPSocketTest : public ::testing::Test {
	public:
		TCPSocketTest()
		{
			net = new Network(nullptr);
			net->Init();
		}
		~TCPSocketTest() {
			if (sock)
				delete sock;
			if (r_buffer)
				delete r_buffer;
		}
		void StartServer(std::string ip, int port, bool blocking = false) {
			sock = new TCPSocket(net);
			int ret = sock->Server(ip, port, blocking);
			if (!ret)
				started = true;
		}
		int OnSent(int result) {
			if (result > 0)
				sent_bytes += result;
			return 0;
		};
		int OnRead(int result) {
			if (result>0)
				read_bytes += result;
			return 0;
		}
		bool Received(char* testbuf, int len) { return !strncmp(r_buffer->reader(), testbuf, len); }
		TCPSocket* sock = nullptr;
		Network* net;
		int tot_connected = 0;
		bool started = false;
		int read_bytes = 0;
		int sent_bytes = 0;
		IOBuffer* r_buffer = nullptr;
	};

	// test the fake server
	TEST_F(TCPSocketTest, StartServer) {
		StartServer("0.0.0.0", 8080);
		EXPECT_TRUE(started) << "Server was not started";
	}

/* this test needs a client that connects (or we would need to add a thread)
TEST_F(TCPSocketTest, ServerAcceptBlocking) {
		StartServer("0.0.0.0", 8080, true);
		if (!sock->Accept(nullptr))
			tot_connected = 1;
		EXPECT_EQ(tot_connected, 1);
	}
	*/

	TEST_F(TCPSocketTest, ServerAccept) {
		StartServer("0.0.0.0", 8080);
		int ret = sock->Accept([&](StreamSocket* new_sock, int err) {
			tot_connected = 1;
			});
		EXPECT_EQ(ret, -1);
		if (ret==-1) {
			bool attempt = false;
			bool cli_con = false;
			do {
				// poll the accept loop
				net->RunLoop();
				if (!attempt) {
					TCPSocket* con_sock = new TCPSocket(net);
					SocketAddress addr;
					addr.SetHost("127.0.0.1", 8080);
					con_sock->Connect(addr, false, [&](int err) -> void {
						cli_con = true;
						return;
						});
					attempt = true;
				}
			} while (tot_connected == 0);
		}
		EXPECT_EQ(tot_connected, 1);
	}

	TEST_F(TCPSocketTest, SendData) {
		char testbuf[] = "0123456789012345678901234567";
		r_buffer = new IOBuffer(1024);

		StartServer("0.0.0.0", 8080);
		int ret = sock->Accept([&](StreamSocket* new_sock, int err) {
			tot_connected = 1;	
			int ret = new_sock->Read(r_buffer, [&](int result)->int {
				return OnRead(result);
				});
			if (ret > 0)
				OnRead(ret);
			});
		EXPECT_EQ(ret, -1);
		if (ret == -1) {
			bool attempt = false;
			bool cli_con = false;
			do {
				// poll the accept loop
				net->RunLoop();
				if (!attempt) {
					TCPSocket* cli_sock = new TCPSocket(net);
					SocketAddress addr;
					addr.SetHost("127.0.0.1", 8080);
					cli_sock->Connect(addr, false, [&](int err) -> void {
						cli_con = true;
						IOBuffer s_buffer;
						s_buffer.Write(testbuf, strlen(testbuf));
						int ret = cli_sock->Write(&s_buffer, [&](int err)->int {
							return OnSent(err);
							});
						if (ret > 0)
							OnSent(ret);
						return;
						});
					attempt = true;
				}
			} while (read_bytes == 0);
		}
		EXPECT_EQ(tot_connected, 1);
		EXPECT_TRUE(Received(testbuf, (int)strlen(testbuf)));
	}

}