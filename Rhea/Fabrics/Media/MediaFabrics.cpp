#include "MediaFabrics.h"
#include <Components/SimpleParser/Type.h>
#include "ImageResource.h"
#include "ImageLoader.h"
#include "Application.h"

namespace Rhea
{
	void MediaFabrics::Register(Application* app)
	{
		SimpleParser::Type::registerType<ImageResource>("//meworla/texture");
		SimpleParser::Type::registerType<ImageResource>("//meworla/image");
		app->getResourceLoader()->registerLoader<ImageLoader>("image/*");		
	}
}