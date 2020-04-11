#include <gmock/gmock.h>
#include "../IRequestHandler.h"

using Hermes::Request;
using Hermes::Response;

class RequestHandlerMock : public Hermes::IRequestHandler
{
public:    
    MOCK_METHOD(void, onRequest, (std::shared_ptr<Request> request, std::shared_ptr<Response> response), (override));
    MOCK_METHOD(void, onData, (unsigned char* chunk, size_t chunk_len), (override));
    MOCK_METHOD(void, onEnd, (), (override));
    MOCK_METHOD(void, onRequestEnd, (), (override));
};
