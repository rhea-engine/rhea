#include <stdio.h>
#include <Psio/Net/Network.h>
#include <Psio/Log.h>
#include "SSLSocket.h"

#ifdef WIN32
#else
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <string.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

Psio::TCPSocket* sock;


int create_socket(int port)
{
    int s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(s, 1) < 0) {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    return s;
}



SSL_CTX* create_context()
{
    const SSL_METHOD* method;
    SSL_CTX* ctx;

    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

int serr(const char* str, size_t len, void* u)
{
    printf(str);
    return 0;
}

void configure_context(SSL_CTX* ctx)
{
    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, "C:/Users/baume/source/data/ssl/domain.crt", SSL_FILETYPE_PEM) <= 0) {
        //ERR_print_errors_cb(&serr, NULL);
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "C:/Users/baume/source/data/ssl/domain_p.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv)
{
    //    int sock;
    Psio::TCPSocket* sock_;
    SSL_CTX* ctx;
    Psio::Network* net = new Psio::Network();
    net->Init();

    ctx = create_context();

    configure_context(ctx);

    //sock = create_socket(4433);
    sock_ = new Psio::TCPSocket(net);
    int ret = sock_->Server("0.0.0.0", 4433, false);

    /* Handle connections */

    struct sockaddr_in addr;
    unsigned int len = sizeof(addr);
    const char reply[] = "HTTP/1.1 200 OK\rContent-Length: 7\r\n\r\nsomebody\n";

    ret = sock_->Accept([&](Psio::TCPSocket* new_sock, int err) {
        if (err) {
            DLOG(LOG_DEBUG) << "Accept failed with err: " << err;
            return;
        }
        DLOG(LOG_DEBUG) << "accepted";
        std::unique_ptr<Psio::TCPSocket> tp_sock(new_sock);
        Psio::SSLSocket* s_sock = new Psio::SSLSocket(net, std::move(tp_sock), ctx);

        int ret_ssl = s_sock->SSLAccept([&,s_sock](int err) {
            if (!err) {
                DLOG(LOG_DEBUG) << "ssl accepted";
                Psio::IOBuffer *inbuf = new Psio::IOBuffer(2048);
                // wait for input
                s_sock->Read(inbuf, [&, inbuf, s_sock](int err) {
                    Psio::IOBuffer outbuf(2048); 
                    if (err >= 0) {
                        size_t size = inbuf->Remaining();
                        std::string s = { inbuf->reader(), size };
                        DLOG(LOG_DEBUG) << "received:\n" << s << "===";
                        outbuf.Write((char*)reply, sizeof(reply));
                        int ret = s_sock->Write(&outbuf, [&, s_sock](int err) {
                            if (!err)
                                DLOG(LOG_DEBUG) << "reply written";
                            s_sock->Close();
                            delete s_sock;
                            delete inbuf;
                            });   
                        if (ret > 0) {
                            DLOG(LOG_DEBUG) << "reply written(1)";
                            s_sock->Close();
                            delete s_sock;
                            delete inbuf;
                        }
                        else if (ret != -1) {
                            DLOG(LOG_DEBUG) << "reply writing failed";
                            s_sock->Close();
                            delete s_sock;
                            delete inbuf;
                        }
                    } else {
                        s_sock->Close();
                        delete s_sock;
                        delete inbuf;
                    }
                    });

            }
            else {                
                s_sock->Close();
                delete s_sock;
            }
            });
        if (ret_ssl != SSL_ERROR_NONE && ret_ssl != SSL_ERROR_WANT_READ && ret_ssl != SSL_ERROR_WANT_WRITE) {
            // accept failed
            int winerr = WSAGetLastError();
            LOG(LOG_ERROR) << "SSLAccept failed: " << ret_ssl << " winerr=" << winerr;
            ERR_print_errors_fp(stderr);
            s_sock->Close();
            delete s_sock;
        }
        });
    while (1) {
        net->RunLoop();
    }

    // release resources
    SSL_CTX_free(ctx);
    delete sock_;
    delete net;
    
}