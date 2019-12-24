#ifndef POSEIDON_NODE_H
#define POSEIDON_NODE_H

#include <vector>

#include "Animatable.h"
#include "Position.h"

namespace Poseidon
{
	/** A node is an item in the scene tree
		this has a position that contains the 3d functions		 
	*/
	class Node
	{
	protected:
		char* m_node_name = "(unknwon)";
		Node* m_parent_node = 0;
		int depth = 0;
		std::vector<Node*> children;
		Animatable* m_obj;
		Position* m_position = 0;
		int m_position_type = 1;
	public:
		Node(int position_type = 0);
		~Node();

		void setName(char* name);
		void addChild(Node* child_node);
		Node* createChild();
		virtual bool update();
		void attach(Animatable* obj);
		Animatable* get_obj() { return m_obj; }

		Position* get_position() { return m_position; }
	};
}

#endif // !POSEIDON_NODE_H