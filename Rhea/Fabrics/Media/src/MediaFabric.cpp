#include "Rhea/Media/MediaFabric.h"
#include "Rhea/Media/ImageLoader.h"
#include "Rhea/Application.h"

namespace rhea
{
	void MediaFabric::Register(Application* app)
	{		
		app->getResourceLoader()->registerLoader<ImageLoader>("image/*");		
	}
}