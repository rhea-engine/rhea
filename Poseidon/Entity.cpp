#include "Entity.h"
#include "Viewport.h"

namespace Poseidon
{
	void Entity::set_mesh(Mesh* node)
	{
		m_node = node;
	}

	bool Entity::onUpdate()
	{
		return false;
	}	
}