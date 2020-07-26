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
		virtual ~Node();

		void setName(char* name);
		
		// adding a child
		void addChild(Node* child_node);

		// remove a child
		void removeChild(Node* child_node);

		// get parent - returns NULL for top
		Node* getParent() { return m_parent_node; }
		Node* createChild();
		virtual bool update();
		virtual void updateImpl();

		// position related

		// get position or if empty the nearest parents
		Position* get_position();
		Position* get_derived_position();

		// create a position of the given type
		void createPosition(int position_type);

		// set the coord space 0: world, 1: parent
		void setCoordSpace(int space) { m_coord_space = space; }

		// we need an update
		void needUpdate();

	protected:	
		char* m_node_name = "(unknwon)";
		Node* m_parent_node = 0;
		int depth = 0;
		std::vector<Node*> children;
		Position* m_position = NULL;
		Position* m_derived_position = NULL;
		int m_coord_space = 0;
		bool m_need_update = false;
		bool m_in_updating = false;
		bool m_derived_valid = false;

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