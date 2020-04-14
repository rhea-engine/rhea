#include <exception>

#include "Hippios/Engine.h"
#include "Poseidon/Core.h"
#include "Orion/Orion.h"
#include "Orion/ResourceStream.h"
#include "Poseidon/SceneManager.h"

#include "ResourceLoader.h"

#include "Application.h"

namespace Rhea
{
	void Application::init()
	{
		core = new Poseidon::Core();
		m_fs = new Orion::Orion(m_appname);
	}

	void Application::setup()
	{
		core->init();
	}

	int Application::loadScene(const std::string resourceId)
	{
		return m_fs->getResourceByRid(resourceId, this);
	}

#define CHUNK_LEN 8192

	void Application::onResourceReady(Orion::Resource* res)
	{
		// for now we assume a scene file
		char* buf = (char*)malloc(CHUNK_LEN);
		char* temp;
		size_t buflen = CHUNK_LEN;
		size_t read;
		size_t size = 0;

		Orion::ResourceStream* rs = res->Open(1);
		read = rs->Read(buf, CHUNK_LEN);
		size += read;
		while (read == CHUNK_LEN) {
			buflen += CHUNK_LEN;
			temp = (char*)realloc(buf, buflen);
			if (temp != NULL)
				buf = temp;
			else {
				printf("Could not allocate memory in Application::onResourceReady\n");
				free(buf);
				return;
			}
			read = rs->Read(buf, CHUNK_LEN);
			size += read;
		}
		// now do something with the file

		// show splash screen (manually)

		ResourceLoader loader;
		loader.load(core);

		// cleanup
		free(buf);
	}


	void Application::onResourceFailed(Orion::ResourceRequest::Error reason)
	{
		printf("Resource loading failed\n");
	}

	int Application::loop()
	{
		// start the loop
		//int ret = core->loop();
		int ret = 0;

		while (!ret) {
			ret = core->iterate();
		}

		return ret;
	}

	
	void Application::close()
	{

	}
}