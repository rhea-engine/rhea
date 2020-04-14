#include "Node.h"

namespace Poseidon
{
	Node::~Node()
	{
		if (m_position)
			delete m_position;
	}

	void Node::setName(char *name)
	{
		m_node_name = name;
	}

	void Node::addChild(Node* child_node)
	{
		child_node->m_parent_node = this;
		child_node->depth = depth + 1;
		children.push_back(child_node);
	}

	Node* Node::createChild()
	{
		Node* node = new Node();
		addChild(node);
		return node;
	}

	bool Node::update()
	{
		// iterate of children and call update
		for (std::vector<Node*>::iterator it = children.begin(); it != children.end(); ++it)
		{
			Node* node = (Node*)*it;
			if (node->update())
				return true;
		}
		return false;
	}

	Position* Node::get_position()
	{
		if (m_position)
			return m_position;
		// check for pos in parent
		Node* p = this;
		while (p->m_parent_node) {
			p = p->m_parent_node;
			if (p->m_position)
				return p->m_position;
		}
		return NULL;
	}

	void Node::createPosition(int position_type)
	{
		if (m_position)
			delete m_position;
		m_position = new Position();
	}
}