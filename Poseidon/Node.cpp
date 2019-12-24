#include "Node.h"

namespace Poseidon
{
	Node::Node(int position_type) : m_obj(nullptr)
	{
		m_position_type = position_type;
		switch (m_position_type) {
		case 1: // 3D
			m_position = new Position();
			break;
		}
	}

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
		Node* node = new Node(m_position_type);
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
		if (m_obj)
			m_obj->onUpdate();
		return false;
	}

	void Node::attach(Animatable* obj)
	{
		m_obj = obj;
		obj->set_node(this);
	}	
}