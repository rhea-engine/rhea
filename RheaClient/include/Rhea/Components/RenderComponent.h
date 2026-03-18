#ifndef RHEA_RENDERCOMPONENT_H
#define RHEA_RENDERCOMPONENT_H

namespace Poseidon
{
	class Node;
	class RenderContext;
	class Mesh;
}
namespace Rhea
{
	class Mesh;
	class Material;
	class Skeleton;

	/** GPU Reendering component, holds the mesh and material information of an entity
	*/
	class RenderComponent : public Component
	{
		RECLASS(RenderComponent, Component, "//meworla/rendercomponent")
	public:
		RenderComponent(std::string name, std::string type_name) :			
			entity(nullptr),
			ctx(nullptr),
			gpuMesh(nullptr),
			skeleton(nullptr),
			Component(name, type_name)
		{}

		Poseidon::Node* entity = nullptr;        // non-owning (Poseidon owns)		
		Poseidon::RenderContext* ctx = nullptr;  // non-owning (Poseidon owns)

		// Cached asset GPU resources (non-owning, owned by caches)
		Poseidon::Mesh* gpuMesh = nullptr;       // Optional (nullptr für Lights)
		Skeleton* skeleton = nullptr;            // Optional (nur für skeletal meshes)


	};
}

#endif // !RHEA_RENDERCOMPONENT_H
