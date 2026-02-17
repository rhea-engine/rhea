#ifndef RHEA_RENDERCOMPONENT_H
#define RHEA_RENDERCOMPONENT_H

namespace Rhea
{
	class Mesh;
	class Material;
	class Skeleton;
	class GraphicsMesh;

	class RenderComponent : public Component
	{
		RECLASS(RenderComponent, Component, "//meworla/rendercomponent")
	public:
		RenderComponent(std::string name, std::string type_name) :			
			mesh(nullptr),
			skeleton(nullptr),
			gfxMesh(nullptr),
			Component(name, type_name)
		{}

		Mesh* getMesh() { return mesh; }
		void setMesh(Mesh* m) { mesh = m; }
		GraphicsMesh* getGraphicsMesh() { return gfxMesh; }

	public:
		bool hidden = false;           // optional local override
		Mesh* mesh; // resolved asset (client)
		Skeleton* skeleton; // cached pointer (non-owning!)
		GraphicsMesh* gfxMesh; // cached element (non-owning!)
	};
}

#endif // !RHEA_RENDERCOMPONENT_H
