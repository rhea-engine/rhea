#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../Http.h"
#include "ConnectionMock.h"
#include "RequestHandlerMock.h"
#include "RequestHandlerFactoryMock.h"

using ::testing::AtLeast;
using ::testing::_;
using ::testing::Return;
using ::testing::Invoke;
using ::testing::Action;
using ::testing::WithArg;
using ::testing::WithArgs;
using ::testing::DoAll;
using ::testing::InvokeArgumentActionP2;

/*
onData side effects:
- wrong input -> connection->error(Bad Request)
- after headers are sent - call connection->getRequestHandler()->onRequest(req,res)
- if onRequest returns true: req->onEnd()
- else call for further data: req->onData
*/

ACTION_P2(setrh, rh, req) { req->setRequestHandler(rh); };

TEST(Http, OneSingleCommandNoHeader) {

    // setup
    ConnectionMock conn;
    Hermes::Http http;
    RequestHandlerFactoryMock rhf;
    RequestHandlerMock rh;

    conn.setRequestHandler(&rhf);

    // buffer
    unsigned const char buf[] = "quit\r\n\r\n";

    //EXPECT_CALL(rh, onRequest(_, _)).Times(AtLeast(1)).WillRepeatedly(DoAll(setrh(&rh, arg1)));
    EXPECT_CALL(rhf, onRequest(_)).Times(AtLeast(1)).WillRepeatedly(Invoke([&](std::shared_ptr<Request> request) { return &rh; }));
    //EXPECT_CALL(rh, onRequest(_, _)).Times(AtLeast(1)).WillRepeatedly(WithArgs<&rh, 0>(setrh)
    EXPECT_CALL(rh, onEnd()).Times(AtLeast(1));

    http.onConnection(&conn);
    http.onData((unsigned char*)buf, strlen((char*)buf));
    
}

TEST(Http, OneSingleCommandOneHeader) {

    // setup
    ConnectionMock conn;
    Hermes::Http http;
    RequestHandlerFactoryMock rhf;
    RequestHandlerMock rh;

    conn.setRequestHandler(&rhf);

    // buffer
    unsigned const char buf[] = "quit\r\nAccept: all\r\n\r\n";

    EXPECT_CALL(rhf, onRequest(_)).Times(AtLeast(1)).WillRepeatedly(Invoke([&](std::shared_ptr<Request> request) { return &rh; }));
    EXPECT_CALL(rh, onEnd()).Times(AtLeast(1));

    http.onConnection(&conn);
    http.onData((unsigned char*)buf, strlen((char*)buf));

}

TEST(Http, TwoSingleCommandNoHeader) {

    // setup
    ConnectionMock conn;
    Hermes::Http http;
    RequestHandlerFactoryMock rhf;
    RequestHandlerMock rh;

    conn.setRequestHandler(&rhf);

    // buffer
    unsigned const char buf[] = "hello\r\n\r\nquit\r\n\r\n";

    EXPECT_CALL(rhf, onRequest(_)).Times(AtLeast(1)).WillRepeatedly(Invoke([&](std::shared_ptr<Request> request) { return &rh; }));
    EXPECT_CALL(rh, onEnd()).Times(AtLeast(2));

    http.onConnection(&conn);
    http.onData((unsigned char*)buf, strlen((char*)buf));

}
