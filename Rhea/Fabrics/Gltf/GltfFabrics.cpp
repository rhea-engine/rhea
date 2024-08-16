#include <Rhea/Application.h>
#include <Orion/Mime.h>

#include "GltfFabrics.h"
#include "GlbLoader.h"
#include "GltfScene.h"
#include "GltfMesh.h"

void Rhea::GltfFabrics::Register(Application* app)
{
	// register gltf loader	
	app->getResourceLoader()->registerLoader<GltfLoader>("model/gltf+json");
	app->getResourceLoader()->registerLoader<GlbLoader>("model/gltf-binary");

	// register mime for gltf
	Orion::MIME::RegisterExtenstion("model/gltf+json", "gltf");
	Orion::MIME::RegisterExtenstion("model/gltf-binary", "glb");
	
	// the resource scene file
	SimpleParser::Type::registerType<GltfScene>("//meworla/gltf");

	// importers
	SimpleParser::Type::registerType<GltfMesh>("//meworla/gltfmesh");
}
