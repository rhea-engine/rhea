/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include "Rhea/ObjFile/ObjFabric.h"
#include "Rhea/Application.h"
#include <Rhea/ObjFile/ObjLoader.h>
#include <Rhea/Resources/TextLoader.h>

namespace rhea
{
	void ObjFabric::Register(Application* app)
	{
		// register wavefront types
		auto* resource_loader = app->getResourceLoader();
		resource_loader->registerLoader<TextLoader>("model/obj");
		resource_loader->registerLoader<TextLoader>("model/mtl");
		simpleparser::Type::registerType<ObjLoader>("//meworla/wavefront");
	}
}