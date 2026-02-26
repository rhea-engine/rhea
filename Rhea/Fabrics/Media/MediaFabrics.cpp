#include "MediaFabrics.h"
#include <SimpleParser/Type.h>
#include "ImageResource.h"
#include "ImageLoader.h"
#include "Rhea/Application.h"
#include "CubeMap.h"

namespace Rhea
{
	void MediaFabrics::Register(Application* app)
	{
		ImageResource::_register_ImageResource();
		SimpleParser::Type::registerType<ImageResource>("//meworla/texture");
		CubeMap::_register_CubeMap();
		app->getResourceLoader()->registerLoader<ImageLoader>("image/*");		
	}
}