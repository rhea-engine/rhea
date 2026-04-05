#ifndef FORTYTWO_H
#define FORTYTWO_H

#include <Rhea/Application.h>
#include <Rhea/Network/ServerService.h>

class FortyTwo : public rhea::Application
{
public:
	FortyTwo() : 
		m_server_service(nullptr),
		m_port(4242)
	{}
	void init(int argc, char* argv[]);
	void setServer(std::string scene);

	void setPort(std::string port);

	// start the setup process
	void setup();

	// listen to actions
	bool onFrameEnd();

	void onScene(rhea::Scene* scene);

protected:
	std::string m_server_name;
	rhea::ServerService* m_server_service;
	uint16_t m_port;
};

#endif // !FORTYTWO_H
