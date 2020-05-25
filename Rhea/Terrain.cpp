#include "Terrain.h"
#include "Application.h"
#include "ImageResource.h"
#include "Unit.h"

namespace Rhea
{
	void Terrain::load(ReadyHandler handler)
	{
		Item::load([&](ReqError err) {
			std::string path = getValue("heightmap");
			m_app->loadResource(path, "//meworla/heightmap", [&](Unit *unit, Orion::ResourceRequest::Error err) {
				if (unit) {
					addChild(unit);
					handler(ReqError::OK);
				}
				else
					handler(err);				
				});
			});
	}

	Unit* Terrain::createType(std::string name, std::string type)
	{
		Unit* ret = (Unit*)new Terrain(name, type);
		return ret;
	}
}