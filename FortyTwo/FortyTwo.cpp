#include "World.h"

#include <string>

int main(int argc, char* argv[])
{
	World* world = new World();
	world->setAppname("FortyTwo");
	int argi = 1;

	world->init();

	while (argi < argc) {
		if (!strcmp(argv[argi], "-key")) {
			argi++;
			//set the ssl key
		}
		else if (!strcmp(argv[argi], "-d")) {
			argi++;
			world->setAppDir(argv[argi]);
		}
		else {
			world->setScene(argv[argi]);
		}
		argi++;
	}

	world->setup();

	world->loop();

	world->close();
}
