#ifndef RHEA_RENDERCOMPONENT_H
#define RHEA_RENDERCOMPONENT_H

#include <Rhea/Fabrics/3D/Mesh.h>

namespace Rhea
{
	class Item;

	class RenderComponent
	{
	public:
		RenderComponent(Item* item) :
			m_item(item),
			m_hidden(false),
			m_material(nullptr),
			m_mesh(nullptr)
		{}

		Item* getOwner() { return m_item; }

		// visibility
		void hide() { m_hidden = true; }
		void show() { m_hidden = false; }
		bool isVisible() { return !m_hidden; }

		// material
		Material* getMaterial() { return m_material; }
		void setMaterial(Material* mat) { m_material = mat; }

		// mesh
		Mesh* getMesh() { return m_mesh; }
		void setMesh(Mesh* mesh) { m_mesh = mesh; }

	protected:
		Item* m_item; // my owner
		bool m_hidden;
		Material* m_material;
		Mesh* m_mesh;
	};
}

#endif // !RHEA_RENDERCOMPONENT_H
