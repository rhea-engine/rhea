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

	class RenderComponent : public Component
	{
		RECLASS(RenderComponent, Component, "//meworla/rendercomponent")
	public:
		RenderComponent(std::string name, std::string type_name) :			
			mesh(nullptr),
			entity(nullptr),
			Component(name, type_name)
		{}

		Mesh* getMesh() { return mesh; }
		void setMesh(Mesh* m) { mesh = m; }		

	public:
		Mesh* mesh = nullptr;                    // resolved asset unit (optional)
		Poseidon::Node* entity = nullptr;        // non-owning (Poseidon owns)		
		Poseidon::RenderContext* ctx = nullptr;  // non-owning (Poseidon owns)

		// Cached asset GPU resources (non-owning, owned by caches)
		Poseidon::Mesh* gpuMesh = nullptr;

		Rhea::Skeleton* skeleton = nullptr;

	};
}

#endif // !RHEA_RENDERCOMPONENT_H
