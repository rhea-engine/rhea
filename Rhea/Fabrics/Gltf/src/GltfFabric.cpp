/*
 * Copyright (c) 2019-2026 Meworla
 *
 * Licensed under the MIT License.
 * See LICENSE file in the project root for full license information.
 */

#include <Rhea/Application.h>
#include <Orion/Mime.h>

#include <Rhea/Gltf/GltfFabric.h>
#include <Rhea/Gltf/GlbLoader.h>
#include <Rhea/Gltf/GltfDocument.h>
#include <Rhea/Gltf/GltfMesh.h>
#include <Rhea/Gltf/GltfGeometry.h>

void rhea::GltfFabric::Register(Application* app)
{
	// register gltf loader	
	app->getResourceLoader()->registerLoader<GltfLoader>("model/gltf+json");
	app->getResourceLoader()->registerLoader<GlbLoader>("model/gltf-binary");

	// register mime for gltf
	orion::MIME::RegisterExtenstion("model/gltf+json", "gltf");
	orion::MIME::RegisterExtenstion("model/gltf-binary", "glb");
	
	// the resource scene file
	simpleparser::Type::registerType<GltfDocument>("//meworla/gltf");

	// importers
	simpleparser::Type::registerType<GltfMesh>("//meworla/gltfmesh");
	simpleparser::Type::registerType<GltfGeometry>("//meworla/gltfgeometry");
}
