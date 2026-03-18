#ifndef POSEIDON_ENTITY_H
#define POSEIDON_ENTITY_H

#include "Animatable.h"
#include "Mesh.h"

namespace Poseidon
{
	class Material;

	class Entity : public Animatable
	{
	protected:
		Mesh* m_mesh = 0;
	public:
		Entity();
		void set_mesh(Mesh* mesh);
		Mesh* get_mesh() { return m_mesh; }		
		bool onUpdate();

		void cullPhase(RenderQueue* queue, Camera* camera);

		// get render input
		void getRenderInput(size_t mesh_idx, Hippios::VertexBuffer** pvb, Hippios::IndexBuffer** pib, size_t& num_indices);

		// get the Material
		Material *getMaterial();
		void setMaterial(Material* material);

		// hide entity
		void hide() { m_hidden = true; }
		
		// show entity
		void show() { m_hidden = false; }
	
	protected:
		size_t m_vertex_size;
		Material* m_material;
		bool m_hidden;
	};
}

#endif // !POSEIDON_ENTITY_H
