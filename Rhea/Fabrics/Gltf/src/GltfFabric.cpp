#include <Rhea/Application.h>
#include <Orion/Mime.h>

#include <Rhea/Gltf/GltfFabric.h>
#include <Rhea/Gltf/GlbLoader.h>
#include <Rhea/Gltf/GltfScene.h>
#include <Rhea/Gltf/GltfMesh.h>

void rhea::GltfFabric::Register(Application* app)
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
