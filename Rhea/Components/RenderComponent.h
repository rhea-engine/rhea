#ifndef RHEA_RENDERCOMPONENT_H
#define RHEA_RENDERCOMPONENT_H

#include <Rhea/Fabrics/3D/Mesh.h>

namespace Rhea
{
	class RenderComponent : public Component
	{
		MECLASS(RenderComponent, Component)
	public:
		RenderComponent(std::string name, std::string type_name) :
			m_hidden(false),
			m_mesh(nullptr),
			Component(name, type_name)
		{}

		// visibility
		void hide() { m_hidden = true; }
		void show() { m_hidden = false; }
		bool isVisible() { return !m_hidden; }

		// mesh
		Mesh* getMesh() { return m_mesh; }
		void setMesh(Mesh* mesh) { m_mesh = mesh; }

	protected:
		bool m_hidden;
		Mesh* m_mesh;
	};
}

#endif // !RHEA_RENDERCOMPONENT_H
