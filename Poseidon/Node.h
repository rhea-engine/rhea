#ifndef POSEIDON_NODE_H
#define POSEIDON_NODE_H

#include <vector>

#include "Position.h"

namespace Poseidon
{
	/** A node is an item in the scene tree	 
	*/
	class Node
	{
	protected:
		char* m_node_name = "(unknwon)";
		Node* m_parent_node = 0;
		int depth = 0;
		std::vector<Node*> children;
		Position* m_position = NULL;
	public:
		Node() :
			m_parent_node(NULL)
		{}
		~Node();

		void setName(char* name);
		void addChild(Node* child_node);
		// get parent - returns NULL for top
		Node* getParent() { return m_parent_node; }
		Node* createChild();
		virtual bool update();

		// position related

		// get position or if empty the nearest parents
		Position* get_position();

		// create a position of the given type
		void createPosition(int position_type);
	};
}

#endif // !POSEIDON_NODE_H