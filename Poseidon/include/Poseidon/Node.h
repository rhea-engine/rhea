#ifndef POSEIDON_NODE_H
#define POSEIDON_NODE_H

#include <vector>
#include <unordered_map>
#include <string>

#include <Rhea/Core/Position.h>
#include <Rhea/Element.h>
#include <Rhea/Unit.h>
#include <Rhea/Core/Transform.h>

using rhea::Position;

namespace Poseidon
{
	/** A node is an item in the scene tree	 
	*/
	class Node : public rhea::Element
	{
	public:
		Node() 
		{}
		virtual ~Node();

		// position related

		// get position
		Position* get_position();
		Position* get_derived_position();

		// set the coord space 0: world, 1: parent
		void setCoordSpace(int space) { m_coord_space = space; }

		void setTransform(rhea::Transform* transform) { m_transform = transform; }
		rhea::Transform* getTransform() { return m_transform; }
	
	protected:	
		
		int m_coord_space = 0;

		rhea::Transform* m_transform = nullptr;

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