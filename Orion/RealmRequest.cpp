#include "RealmRequest.h"
#include "Orion.h"
#include <nlohmann/json.hpp>

namespace Orion
{
	RealmRequest::RealmRequest(Orion* orion,std::deque<std::string>& iris) :
		m_error(0),
		m_orion(orion),
		m_request(nullptr),
		m_iris(iris)
	{

	}
	
	RealmRequest::~RealmRequest()
	{
		if (m_request)
			delete m_request;
	}
	
	void RealmRequest::Lookup(std::string realm_name, RealmHandler handler)
	{
		m_request = new Psio::UrlRequest(m_orion->getIO());		
		// call UrlRequest
		m_request->OnResponse([handler](Psio::UrlResponse* res) {
			// got a response
			// try reading the body in IOBuffer buf
			Psio::IOBuffer* buf = new Psio::IOBuffer(64 * 1024);
			res->onEnd([buf, handler](size_t bytes_read) {
				// success -  we got the result in body
				// the result is json -> parse it
				std::string response = std::string(buf->data(), bytes_read);
				delete buf;
				try {
					json j = json::parse(response);
					Realm* realm = new Realm(j.get<Realm>());
					handler(realm, 0);
				}
				catch (json::exception e) {
					handler(nullptr, -1);
				}
				});
			res->onError([buf, handler](int err) {
				// failed with an error
				delete buf;
				handler(nullptr, err);
				});
			res->ReadAsync(buf);
			});
		m_request->OnError([handler](int err) {
			handler(nullptr, err);
			});
		if (m_iris.size() < 1) {
			m_iris.push_back("http://localhost/iris/realms");
		}
		std::string iris_server = m_iris.front();
		m_iris.pop_front();

		m_request->Get(iris_server + "/" + realm_name);
	}
	
	void RealmRequest::Cancel()
	{
		if (m_request) {
			// remove the request
			delete m_request;
			m_request = nullptr;
		}
	}
}