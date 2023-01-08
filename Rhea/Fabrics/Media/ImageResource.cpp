#include "ImageResource.h"

#include "Application.h"
#include "Orion/ResourceStream.h"

namespace Rhea
{
	ImageResource::~ImageResource()
	{		
	}

	Unit* ImageResource::createType(std::string name, std::string type)
	{
		Unit* ret = new ImageResource(name, type);
		return ret;
	}
}
