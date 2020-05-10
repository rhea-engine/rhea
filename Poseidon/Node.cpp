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
		child_node->registerNode();
	}

	Node* Node::createChild()
	{
		Node* node = new Node();
		addChild(node);
		return node;
	}

	bool Node::update()
	{
		// update myself
		if (m_need_update) {
			updateImpl();
			m_need_update = false;
		}

		// iterate of children and call update
		for (std::vector<Node*>::iterator it = children.begin(); it != children.end(); ++it)
		{
			Node* node = (Node*)*it;
			if (node->update())
				return true;
		}
		return false;
	}

	void Node::updateImpl()
	{
		// update position
		if (m_parent_node) {

			m_in_updating = true;

			// Update orientation
			const Quaternion& parent_rot = m_parent_node->get_position()->get_rotation();
			Position* src = get_position();
			Position* derived = get_derived_position();
			if (m_coord_space==1)
			{
				// Combine orientation with that of parent
				derived->setOrientation(parent_rot * src->get_rotation());
			}
			else {
				derived->setOrientation(get_position()->get_rotation());
			}

			// Change position vector based on parent's orientation & scale
			Vector3 derived_pos = parent_rot * src->get_worldspace();
			
			// Add altered position vector to parents
			derived_pos += m_parent_node->get_derived_position()->get_worldspace();
			derived->setPosition(derived_pos);

			m_derived_valid = true;
			m_in_updating = false;
		}
		else {
			// root node 
			m_derived_position->setPosition(*get_position());
			m_derived_valid = true;
		}
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
		// if no parent has a position the setup is probably wrong
		// TODO: report it?
		createPosition(1);
		return m_position;
	}

	Position* Node::get_derived_position()
	{
		if (!m_derived_position) {			
			m_derived_position = new Position(NULL);
			m_derived_position->setPosition(*get_position());
		}
		if (!m_derived_valid && !m_in_updating)
			updateImpl();
		return m_derived_position;
	}

	void Node::needUpdate()
	{
		// if already queued no need to queue again
		if (!m_need_update && !m_in_updating) {
			m_need_update = true;
			// all children need update
			for (auto it = children.begin(); it != children.end(); ++it) {
				static_cast<Node*>(*it)->needUpdate();
			}
		}
	}

	void Node::createPosition(int position_type)
	{
		if (m_position)
			delete m_position;
		m_position = new Position(this);

		if (m_derived_position)
			delete m_derived_position;
		m_derived_position = new Position(NULL);
		m_derived_valid = false;
	}

	void Node::registerGroup(std::string group_name, Node* node)
	{
		// we store nothing - this has to be done in children

		// call parents
		if (m_parent_node)
			m_parent_node->registerGroup(group_name, node);
	}
}