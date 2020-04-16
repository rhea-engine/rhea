#ifndef POSEIDON_NODE_H
#define POSEIDON_NODE_H

#include <vector>
#include <unordered_map>
#include <string>

#include "Position.h"

namespace Poseidon
{
	/** A node is an item in the scene tree	 
	*/
	class Node
	{
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

	protected:
		char* m_node_name = "(unknwon)";
		Node* m_parent_node = 0;
		int depth = 0;
		std::vector<Node*> children;
		Position* m_position = NULL;

		// groups of nodes

		// register this node to the groups of the parents it wants to
		virtual void registerNode() {}

		// register child node to group
		virtual void registerGroup(std::string group_name, Node* node);

		// parent nodes store nodes they are interessted in
		std::unordered_map<std::string, std::vector<Node*>> m_groups;
	};
}

#endif // !POSEIDON_NODE_H