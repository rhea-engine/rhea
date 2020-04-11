#include <gmock/gmock.h>
#include "../Connection.h"

class ConnectionMock : public Hermes::Connection
{
public:
    ConnectionMock() : Connection(0, 0) {}
    MOCK_METHOD(void, write, (unsigned char* buf, size_t len), (override));
    MOCK_METHOD(void, close, (), (override));
};
