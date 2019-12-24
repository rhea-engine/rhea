#ifndef POSEIDON_ENTITY_H
#define POSEIDON_ENTITY_H

#include "Animatable.h"
#include "Mesh.h"

namespace Poseidon
{
	class Entity : public Animatable
	{
	protected:
		Mesh* m_mesh = 0;
	public:
		void set_mesh(Mesh* mesh);
		Mesh* get_mesh() { return m_mesh; }		
		bool onUpdate();
	};
}

#endif // !POSEIDON_ENTITY_H
