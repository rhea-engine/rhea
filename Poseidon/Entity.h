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
		void getRenderInput(Hippios::VertexBuffer** pvb, Hippios::IndexBuffer** pib, int& num_indices);

		// get the Material
		Texture* getMaterial();
	protected:
		int m_vertex_size = 0;
		Texture* m_texture = 0;
	};
}

#endif // !POSEIDON_ENTITY_H
