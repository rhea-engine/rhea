/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

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