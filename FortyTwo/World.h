#ifndef FORTYTWO_WORLD_H
#define FORTYTWO_WORLD_H

#include <string>
#include <Rhea/Application.h>
#include "ServerService.h"

class World : public Rhea::Application
{
public:
	World() :
		m_scene_ready(false)
	{}
	void init();
	void setScene(std::string scene);

	// start the setup process
	void setup();

	// listen to actions
	bool onFrameEnd();

	void onScene(Rhea::Scene* scene);

protected:
	std::string m_scene;
	bool m_scene_ready;
	ServerService* m_server_service;

};

#endif