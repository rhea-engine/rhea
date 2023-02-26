#ifndef FORTYTWO_H
#define FORTYTWO_H

#include <Rhea/Application.h>
#include "ServerService.h"

class FortyTwo : public Rhea::Application
{
public:
	FortyTwo() : 
		m_server_service(nullptr)
	{}
	void init();
	void setServer(std::string scene);

	// start the setup process
	void setup();

	// listen to actions
	bool onFrameEnd();

	void onScene(Rhea::Scene* scene);

protected:
	std::string m_server_name;
	ServerService* m_server_service;
	
};

#endif // !FORTYTWO_H
