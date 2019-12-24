#ifndef POSEIDON_SCENE_H
#define POSEIDON_SCENE_H

#include "Node.h"

namespace Poseidon
{
	/** The scene that holds and organises the objects in the world
	*/
	class Scene
	{
	public:
		Node* createNode();
		Node* get_root();
		bool update();
	protected:
		Node* m_tree = 0;	
	};
}
#endif // !POSEIDON_SCENE_H
