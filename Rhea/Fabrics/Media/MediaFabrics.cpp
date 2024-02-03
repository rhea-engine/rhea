#include "MediaFabrics.h"
#include <Components/SimpleParser/Type.h>
#include "ImageResource.h"
#include "ImageLoader.h"
#include "Application.h"
#include "CubeMap.h"

namespace Rhea
{
	void MediaFabrics::Register(Application* app)
	{
		SimpleParser::Type::registerType<ImageResource>("//meworla/texture");
		SimpleParser::Type::registerTypeName<ImageResource>("//meworla/image");
		SimpleParser::Type::registerType<CubeMap>("//meworla/cubemap");
		app->getResourceLoader()->registerLoader<ImageLoader>("image/*");		
	}
}