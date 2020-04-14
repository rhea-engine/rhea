#ifndef POSEIDON_SCENEMANAGER_H
#define POSEIDON_SCENEMANAGER_H

namespace Poseidon
{
	class Node;
	class Viewport;

	/** The scene that holds and organises the objects in the world
	*/
	class SceneManager
	{
	public:
		Node* createNode();
		Node* get_root();

		// update all elements in the tree
		bool update();

		// render all viewports
		bool render();

		// set up the root node
		void setRoot(Node* node) { m_tree = node; }
		
		// helper 
		Viewport* getViewport();
	protected:
		Node* m_tree = 0;	
	};
}
#endif // !POSEIDON_SCENEMANAGER_H
