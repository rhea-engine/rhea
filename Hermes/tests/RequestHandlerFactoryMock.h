#include <gmock/gmock.h>
#include "../IRequestHandlerFactory.h"

using Hermes::Request;
using Hermes::Response;

class RequestHandlerFactoryMock : public Hermes::IRequestHandlerFactory
{
public:
    MOCK_METHOD(Hermes::IRequestHandler*, onRequest, (std::shared_ptr<Request> request), (override));
};
